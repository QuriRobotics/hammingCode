#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

#define BLOCK_SIZE 64
#define DBLOCK_SIZE 28

bool cb(char word, int idx)
{
    return ((word >> idx) & 0b00000001);
}

char dec74byte(char word)
{
    char out = 0;

    bool f[3];

    f[0] = cb(word, 6) ^ cb(word, 4) ^ cb(word, 2) ^ cb(word, 0);
    f[1] = cb(word, 5) ^ cb(word, 4) ^ cb(word, 1) ^ cb(word, 0);
    f[2] = cb(word, 3) ^ cb(word, 2) ^ cb(word, 1) ^ cb(word, 0);
    
    char offset = 7 - (f[0] + f[1] * 2 + f[2] * 4);

    if (offset != 7)
    {
        word ^= 1 << offset;
    }

    out = (word & 0b00000111) | ((word & 0b00010000) >> 1);

    return out;
}

void decode(char* data, char* output)
{
    char hiword, loword;

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        hiword = dec74byte(data[i * 2]);
        loword = dec74byte(data[i * 2 + 1]);

        output[i] = hiword << 4 | loword;
    }
}

int decoder(FILE* r_fifo, FILE* w_fifo, FILE* tracklog)
{
    std::vector<std::string> fileBuf;

    std::string buf(BLOCK_SIZE, '\0');
    long s;

    s = fread(&buf[0], sizeof(char), BLOCK_SIZE * 2, r_fifo);

    while(s)
    {
        //decode(buf, decoded);
        //if (s > 1)
        //    fwrite(decoded, sizeof(char), s/2, w_fifo);
        s = fread(&buf[0], sizeof(char), BLOCK_SIZE, r_fifo);
    }
    return 0;
}
