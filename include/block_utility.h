#pragma once

#include <cstdint>
#include "texture_utility.h"

#define NUM_BLOCK_TYPES 2

using blockIdType = uint8_t;

enum BlockMap: blockIdType{
    air,
    grass
};

enum blockDirectionIndex{
    front,
    back,
    left,
    right,
    bottom,
    top
};

struct BlockInfo{
    TexMap texId[6];
};

inline BlockInfo* blockTexMap;
void generate_data();