#ifndef CHUNK_MANAGEMENT_H
#define CHUNK_MANAGEMENT_H

#include "chunk.h"
#include "chunk_mesh.h"
#include "chunk_utility.h"
#include <string>
#include <memory>
#include <map>
#include <filesystem>
#include <vector>

class ChunkManager{
    size_t header_size;
    std::string chunkDir;
    ChunkErrorCode createRegionFile(int x, int z);
    std::map<std::pair<int, int>, bool>regionFileCache;
    std::map<std::pair<int, int>, std::unique_ptr<ChunkMesh>> staticChunkMeshes;
    std::map<std::pair<int, int>, std::unique_ptr<ChunkMesh>> activeChunkMeshes;
public:

    ChunkManager(const std::string& chunkDir);
    ChunkErrorCode cacheRegionFile(int region_x, int region_z);

    ChunkMesh* getChunkMesh(int chunk_x, int chunk_z);
    ChunkErrorCode deleteChunkMesh(int chunk_x, int chunk_z);
    ChunkErrorCode loadChunkMesh(int chunk_x, int chunk_z, bool setActive);

    ChunkMesh* getActiveChunkMesh(int chunk_x, int chunk_z);
    ChunkErrorCode deleteActiveChunkMesh(int chunk_x, int chunk_z);

    ChunkMesh* getStaticChunkMesh(int chunk_x, int chunk_z);
    ChunkErrorCode deleteStaticChunkMesh(int chunk_x, int chunk_z);

    void getRegionCoordsFromChunkCoords(int chunk_x, int chunk_z, int* region_x, int* region_z);
    void getChunkCoordsFromWorldCoords(int world_x, int world_z, int* chunk_x, int* chunk_z);
    void getRegionCoordsFromWorldCoords(int world_x, int world_z, int* region_x, int* region_z);

    ChunkErrorCode setChunkDir(const std::string& chunkDir);
    std::string getRegionFileName(int region_x, int region_z);
};  

#endif