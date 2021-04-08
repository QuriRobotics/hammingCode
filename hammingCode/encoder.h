#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

#define BLOCK_SIZE 64
#define DBLOCK_SIZE 28

// int32, string(28)
//  \|/     \|/
//   8b  +  56b x7/4ham

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

std::string encodeBlock(uint32_t i, std::string data)
{
	std::string out(BLOCK_SIZE, 0x00);

	char* number = (char*)&i;
	std::string encodedNumber(8, 0x00);
	encode(number, &encodedNumber[0], 4);

	char encodedData[DBLOCK_SIZE * 2];
	encode(&data[0], encodedData, DBLOCK_SIZE);

	/*for (int i = 0; i < 4; i++)
	{
		out[i] = number[i];
	}
	for (int i = 4; i < 32; i++)
	{
		out[i] = data[i - 4];
	}*/
	for (int i = 0; i < 8; i++)
	{
		out[i] = encodedNumber[i];
	}
	for (int i = 8; i < 64; i++)
	{
		out[i] = encodedData[i - 8];
	}

	return out;
}

// i  = * ( long * ) &y;

int encoder(FILE* r_fifo, FILE* w_fifo)
{
	std::vector<std::string> fileBuf;

	std::string buf(DBLOCK_SIZE, '\0');
	long s;

	s = fread(&buf[0], sizeof(char), DBLOCK_SIZE, r_fifo);
	while (s)
	{
		fileBuf.push_back(buf);
		s = fread(&buf[0], sizeof(char), DBLOCK_SIZE, r_fifo);
	}

	std::string dBuff(BLOCK_SIZE, 0x00);

	for (uint32_t i = 0; i < fileBuf.size(); i++)
	{
		dBuff = encodeBlock(i, fileBuf[i]);
		fwrite(&dBuff[0], sizeof(char), BLOCK_SIZE, w_fifo);
	}

	return 0;
}

// fwrite(decoded, sizeof(char), s/2, w_fifo);