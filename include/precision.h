#ifndef PRECISION_H
#define PRECISION_H
#include <cstdint>

using idType = uint8_t;
#define BLOCK_X_SIZE 16
#define BLOCK_Y_SIZE 256
#define BLOCK_Z_SIZE 16

using chunk_header_offset_type = uint32_t;
using chunk_header_length_type = uint32_t;

constexpr int chunks_per_region_side = 32;

#endif