#pragma once

#include <cstdint>

using idType = uint8_t;
#define BLOCK_X_SIZE 16
#define BLOCK_Y_SIZE 16
#define BLOCK_Z_SIZE 16
#define CHUNKS_PER_REGION_SIDE 32

using chunk_header_offset_type = uint32_t;
using chunk_header_length_type = uint32_t;

inline int MathMod(int a, int b){
    return (a % b + b) % b;
}

enum ChunkErrorCode{
    NO_ERROR,
    CHUNK_EMPTY,
    CHUNK_CORRUPTED,
    CHUNK_NOT_FOUND,
    CHUNK_NOT_ACTIVE,
    CHUNK_MESH_CORRUPTED,
    HEADER_CORRUPTED,
    FILE_ERROR,
    COMPRESSION_ERROR,
    DIRECTORY_CREATION_FAILED
};

struct HeaderEntry{
    chunk_header_offset_type offset; 
    chunk_header_length_type length;
};

struct blockData{
    idType id;
};

struct ChunkData{
    int x, z;
    blockData blocks[BLOCK_Y_SIZE][BLOCK_X_SIZE][BLOCK_Z_SIZE];
};

enum blockDirectionIndex{
    up,
    down,
    left,
    right,
    bottom,
    top
};

inline const int neighbor_offsets[6][3] = {
    { 0, 0,-1},  // Front
    { 0, 0, 1},  // Back
    {-1, 0, 0},  // Left
    { 1, 0, 0},  // Right
    { 0,-1, 0},  // Bottom
    { 0, 1, 0}   // Top
};

inline const float blockVertices[6][48] = {
    // as seen down onto the x-z plane
/*
    -z
    |
    |
    |_______ +x

    up is towards you, +ve Y axis
    down is away from you, -ve Y axis
*/
    // Front
    {
        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
    },

    // Back
    {
        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
    },

    // Left
    {
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
    },

    // Right
    {
         0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
    },

    // Bottom
    {
        -0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
    },

    // Top
    {
        -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,    0.0f, 1.0f
    }
};