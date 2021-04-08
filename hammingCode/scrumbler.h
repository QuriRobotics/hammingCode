#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 1000

void scrumble(char* data, char* output)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        output[i] = data[i] ^ (1 << (rand()%9));
    }
}

int scrumbler(FILE* r_fifo, FILE* w_fifo)
{
    char buf[BLOCK_SIZE];
    char scrumbled[BLOCK_SIZE];
    long s;
    s = fread(buf, sizeof(char), BLOCK_SIZE, r_fifo);
    for (; s > 0;)
    {
        scrumble(buf, scrumbled);
        if (s > 1)
            fwrite(scrumbled, sizeof(char), s, w_fifo);
        s = fread(buf, sizeof(char), BLOCK_SIZE, r_fifo);
    }
    return 0;
}
