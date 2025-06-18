#ifndef CHUNK_H
#define CHUNK_H

#include "chunk_utility.h"

struct HeaderEntry{
    chunk_header_offset_type offset; 
    chunk_header_length_type length;
};

struct blockData{
    idType id;
};

struct Chunk{
    int x, z;
    blockData blocks[BLOCK_Y_SIZE][BLOCK_X_SIZE][BLOCK_Z_SIZE];
};

#endif