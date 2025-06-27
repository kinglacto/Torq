#pragma once

#include "chunk_utility.h"
#include "shader.h"
#include <vector>
#include "vertex.h"
#include <memory>
#include <cmath>

class ActiveChunk{
public:
    int vertexCount;
    std::pair<int, int> chunk_coords;
    bool remeshNeeded;
    bool chunkEverUpdated;

    std::shared_ptr<ChunkData> chunk;
    std::vector<TextureVertex> vertices;

    explicit ActiveChunk(std::shared_ptr<ChunkData> chunk);
    ChunkErrorCode setBlock(int x, int y, int z, blockData data);
};