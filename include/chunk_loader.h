#pragma once

#include "chunk_utility.h"
#include "worldgen.hpp"
#include <string>
#include <memory>
#include <map>
#include <filesystem>
#include <vector>
#include <unordered_set>

struct PairHash {
  size_t operator()(std::pair<int,int> const& p) const noexcept {
    // Pack two 32‑bit ints into one 64‑bit value, then hash
    uint64_t key = (uint64_t(uint32_t(p.first)) << 32) | uint32_t(p.second);
    return std::hash<uint64_t>()(key);
  }
};

class ChunkLoader{
    static constexpr size_t header_size = CHUNKS_PER_REGION_SIDE * CHUNKS_PER_REGION_SIDE * 
    sizeof(HeaderEntry);
    std::string chunkDir;
    ChunkErrorCode createRegionFile(int x, int z);
    std::unordered_set<std::pair<int, int>, PairHash> ExistingFiles;
public:
    std::shared_ptr<Terrain> terrain;
    
    ChunkLoader(const std::string& chunkDir);
    ChunkErrorCode cacheRegionFile(int region_x, int region_z);

    std::unique_ptr<ChunkData> getChunk(int chunk_x, int chunk_z, ChunkErrorCode* error);

    ChunkErrorCode writeChunk(ChunkData* chunkData);

    ChunkErrorCode setChunkDir(const std::string& chunkDir);
    std::string getRegionFileName(int region_x, int region_z);
};