#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <math.h>

#define BLOCK_SIZE 128
#define DBLOCK_SIZE 52
#define INDEX_OFFSET 16

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

void decode(char* data, char* output, int twosize)
{
    char hiword, loword;

    for (int i = 0; i < twosize/2; i++)
    {
        hiword = dec74byte(data[i * 2]);
        loword = dec74byte(data[i * 2 + 1]);

        output[i] = hiword << 4 | loword;
    }
}

std::string decodeBlock(std::string data)
{
    std::string decdata(DBLOCK_SIZE, 0x00);

    for (int i = 0; i < DBLOCK_SIZE; i++)
    {
        decdata[i] = data[2 + 4 + i];
    }

    return decdata;
}

int decoder(FILE* r_fifo, FILE* w_fifo, FILE* tracklog)
{
    std::vector<std::string> fileBuf(1024);

    std::string buf(BLOCK_SIZE, 0x00);
    std::string decbuf(BLOCK_SIZE, 0x00);
    std::string databuf(DBLOCK_SIZE, 0x00);
    long s;

    s = fread(&buf[0], sizeof(char), 1, r_fifo);

    uint32_t maxI = 0;

    // 0xFF 0xFF | uint32_t | string(52) | uint32_t | 0xAA 0xAA 

    while(s)
    {
        decode(&buf[0], &decbuf[0], 2);

        if ((decbuf[0] & '\xf0') == '\xf0')
        {
            s = fread(&buf[1], sizeof(char), 1, r_fifo);
            s = fread(&buf[2], sizeof(char), 1 * 2, r_fifo);
            if (s == 0) break;
            decode(&buf[0], &decbuf[0], 4);
            if (decbuf[1] == '\xff')
            {
                s = fread(&buf[4], sizeof(char), BLOCK_SIZE - 2 * 2, r_fifo);
                if (s == 0) break;
                decode(&buf[4], &decbuf[2], BLOCK_SIZE - 2 * 2);

                if (decbuf[2+4+DBLOCK_SIZE+4] == '\xee' && decbuf[2 + 4 + DBLOCK_SIZE + 4 + 1] == '\xee')
                {
                    uint32_t i1 = *(uint32_t*)&decbuf[2];
                    uint32_t i2 = *(uint32_t*)&decbuf[2 + 4 + DBLOCK_SIZE];

                    if (i1 == i2)
                    {
                        uint32_t index = i1 - INDEX_OFFSET;
                        databuf = decodeBlock(decbuf);
                        while (index > fileBuf.size())
                        {
                            fileBuf.resize(fileBuf.size() * 2);
                        }
                        fileBuf[index] = databuf;

                        maxI = std::max(maxI, index);
                        fwrite(&decbuf[0], sizeof(char), BLOCK_SIZE / 2, w_fifo);
                    }
                }
            }
        }

        //if (s > 1)
        //    fwrite(decoded, sizeof(char), s/2, w_fifo);
        //fileBuf.push_back(buf);
        s = fread(&buf[0], sizeof(char), 1, r_fifo);
    }
    
    for(int i = 0; i < 1<<7; i++) fwrite("\x00", sizeof(char), 1, w_fifo);

    //std::string dBuff(DBLOCK_SIZE, 0x00);

    int idx;
    for (idx = 0; idx < maxI; idx++)
    {
        fwrite(&fileBuf[idx][0], sizeof(char), DBLOCK_SIZE, w_fifo);
    }
    int lastsize;
    for (lastsize = DBLOCK_SIZE; lastsize > 0; lastsize--)
    {
        if (fileBuf[idx][lastsize - 1] != '\x00') break;
    }
    fwrite(&fileBuf[idx][0], sizeof(char), lastsize, w_fifo);

    return 0;
}
