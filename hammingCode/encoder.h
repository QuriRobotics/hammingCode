#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 1000

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

void encode(char* data, char* output)
{
	char hiword, loword;

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		hiword = data[i] >> 4;
		loword = data[i] & 0b00001111;

		output[2 * i] = enc74byte(hiword);
		output[2 * i + 1] = enc74byte(loword);
	}
}

int encoder(FILE* r_fifo, FILE* w_fifo)
{
	char buf[BLOCK_SIZE];
	char encoded[BLOCK_SIZE * 2];
	long s;
	s = fread(buf, sizeof(char), BLOCK_SIZE, r_fifo);
	for (; s > 0;)
	{
		encode(buf, encoded);
		fwrite(encoded, sizeof(char), s*2, w_fifo);
		s = fread(buf, sizeof(char), BLOCK_SIZE, r_fifo);
	}
	return 0;
}
