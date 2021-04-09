#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

#define BLOCK_SIZE 128
#define DBLOCK_SIZE 52
#define INDEX_OFFSET 16

// header, int32, string(52), int32, tail

bool checkBit(char word, int idx)
{
	return ((word >> idx) & 0b00000001);
}

void printByte(char word)
{
	for (int i = 7; i >= 0; i--)
	{
		printf("%d", checkBit(word, i));
	}
	printf("\n");
}

// 0b0000abcd
// 0b0ffafbcd
//    12 3

char enc74byte(char word) // word = 0b0000abcd
{
	char out = 0;

	out = word & 0b00000111 | (word & 0b00001000) << 1;

	//printByte(out);

	bool f[3];

	f[0] = checkBit(word, 3) ^ checkBit(word, 2) ^ checkBit(word, 0);
	f[1] = checkBit(word, 3) ^ checkBit(word, 1) ^ checkBit(word, 0);
	f[2] = checkBit(word, 2) ^ checkBit(word, 1) ^ checkBit(word, 0);

	out |= f[0] << 6 | f[1] << 5 | f[2] << 3;

	return out;
}

void encode(char* data, char* output, int size)
{
	char hiword, loword;

	for (int i = 0; i < size; i++)
	{
		hiword = data[i] >> 4;
		loword = data[i] & 0b00001111;

		output[2 * i] = enc74byte(hiword);
		output[2 * i + 1] = enc74byte(loword);
	}
}

std::string encodeBlock(uint32_t i, std::string data, int lastsize = DBLOCK_SIZE)
{
	std::string out(BLOCK_SIZE, 0x00);

	std::string header(2, 0xFF);
	std::string tail(2, 0xEE);

	std::string encodedHeader(4, 0xFF);
	std::string encodedTail(4, 0x00);
	
	encode(&header[0], &encodedHeader[0], 2);
	encode(&tail[0], &encodedTail[0], 2);

	char* number = (char*)&i;
	std::string encodedNumber(8, 0x00);
	encode(number, &encodedNumber[0], 4);

	std::string encodedData(DBLOCK_SIZE * 2, 0x00);
	encode(&data[0], &encodedData[0], lastsize);

	if (0)
	{
		int offset = 0;
		for (int i = offset; i < offset + 2; i++)
		{
			out[i] = header[i - offset];
		}
		offset += 2;
		for (int i = offset; i < offset + 4; i++)
		{
			out[i] = number[i - offset];
		}
		offset += 4;
		for (int i = offset; i < offset + DBLOCK_SIZE; i++)
		{
			out[i] = data[i - offset];
		}
		offset += DBLOCK_SIZE;
		for (int i = offset; i < offset + 4; i++)
		{
			out[i] = number[i - offset];
		}
		offset += 4;
		for (int i = offset; i < offset + 2; i++)
		{
			out[i] = tail[i - offset];
		}
	}
	else
	{
		int offset = 0;
		for (int i = offset; i < offset + 2 * 2; i++)
		{
			out[i] = encodedHeader[i - offset];
		}
		offset += 2 * 2;
		for (int i = offset; i < offset + 4 * 2; i++)
		{
			out[i] = encodedNumber[i - offset];
		}
		offset += 4 * 2;
		for (int i = offset; i < offset + DBLOCK_SIZE * 2; i++)
		{
			out[i] = encodedData[i - offset];
		}
		offset += DBLOCK_SIZE * 2;
		for (int i = offset; i < offset + 4 * 2; i++)
		{
			out[i] = encodedNumber[i - offset];
		}
		offset += 4 * 2;
		for (int i = offset; i < offset + 2 * 2; i++)
		{
			out[i] = encodedTail[i - offset];
		}
	}

	return out;
}

// i  = * ( long * ) &y;

int encoder(FILE* r_fifo, FILE* w_fifo)
{
	std::vector<std::string> fileBuf;

	std::string buf(DBLOCK_SIZE, '\0');
	long s;
	int lastsize = 0;

	s = fread(&buf[0], sizeof(char), DBLOCK_SIZE, r_fifo);
	while (s)
	{
		fileBuf.push_back(buf);
		lastsize = s;
		s = fread(&buf[0], sizeof(char), DBLOCK_SIZE, r_fifo);
	}

	for (int rep = 0; rep < 2; rep++)
	{
		std::string dBuff(BLOCK_SIZE, 0x00);
		uint32_t i;
		for (i = 0; i < fileBuf.size() - 1; i++)
		{
			dBuff = encodeBlock(i + INDEX_OFFSET, fileBuf[i]);
			fwrite(&dBuff[0], sizeof(char), BLOCK_SIZE, w_fifo);
		}
		dBuff = encodeBlock(i + INDEX_OFFSET, fileBuf[i], lastsize);
		fwrite(&dBuff[0], sizeof(char), BLOCK_SIZE, w_fifo);
	}
	/*for (uint32_t i = 0; i < 1 << 12; i++)
	{
		char a = i;
		fwrite(&a, sizeof(char), 1, w_fifo);
	}*/

	return 0;
}

// fwrite(decoded, sizeof(char), s/2, w_fifo);