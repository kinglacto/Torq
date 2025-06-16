#ifndef CHUNK_MANAGEMENT_H
#define CHUNK_MANAGEMENT_H

#include "chunk.h"
#include "precision.h"
#include <string>
#include <memory>
#include <map>
#include <filesystem>

class ChunkManager{
    std::string chunkDir;
    bool regionFileExists(int x, int z);
    std::map<std::pair<int, int>, bool>regionFileCache;
public:
    ChunkManager(const std::string& chunkDir);
    std::unique_ptr<Chunk> load(int x, int z);
    bool write(int x, int z, std::vector<idType>& chunkData);
    bool setChunkDir(const std::string& chunkDir);

    std::string getFileName(int chunk_x, int chunk_z);
};  


#endif