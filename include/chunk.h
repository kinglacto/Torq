#ifndef CHUNK_H
#define CHUNK_H

#include "precision.h"

struct Chunk{
    int x, z;
    idType blocks[BLOCK_X_SIZE][BLOCK_Y_SIZE][BLOCK_Z_SIZE];
};

#endif