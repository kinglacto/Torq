#pragma once

#include <map>
#include <queue>
#include <memory>
#include "active_chunk.h"
#include "chunk_loader.h"
#include "chunk_mesh.h"
#include "shader.h"
#include "texture.h"
#include "block_utility.h"
#include "thread_safe_queue.h"
#include "worldgen.hpp"
#include <glm/glm.hpp>
#include <mutex>
#include <condition_variable>

class ChunkRenderer{
public:
    glm::vec3 worldPos;
    Texture* texture;

    int renderDistance {10};
    int activeRadius {5};

    std::mutex world_pos_mutex;

    std::unordered_set<std::pair<int, int>, PairHash> pendingChunkRequests;
    std::unordered_set<std::pair<int, int>, PairHash> pendingRegionGenerations;

    std::map<std::pair<int, int>, std::shared_ptr<ChunkMesh>> chunkMeshes;
    std::map<std::pair<int, int>, std::shared_ptr<ActiveChunk>> activeChunks;
    std::shared_ptr<ChunkLoader> chunkLoader;

    std::queue<std::pair<std::shared_ptr<ChunkMesh>, std::shared_ptr<ChunkData>>> ChunkMeshQueue;
    std::queue<std::shared_ptr<ActiveChunk>> ActiveChunkRemeshQueue;

    void deleteActiveChunk(const std::pair<int, int>& chunk_coords);
    void deleteActiveChunk(std::shared_ptr<ActiveChunk> activeChunk);

    void deleteChunkMesh(const std::pair<int, int>& chunk_coords);
    void deleteChunkMesh(ChunkMesh* chunkMesh);

    void makeChunkMesh(const std::pair<int, int>& chunk_coords, 
    std::shared_ptr<ChunkData> chunkData);
    void makeChunkActive(const std::pair<int, int>& chunk_coords, 
    std::shared_ptr<ChunkData> chunkData);

    void makeChunkActiveRequest(const std::pair<int, int>& chunk_coords);
    void makeChunkMeshRequest(const std::pair<int, int>& chunk_coords);

    ChunkErrorCode generateStaticMesh(std::shared_ptr<ChunkMesh>& mesh,
        std::shared_ptr<ChunkData>& it);
    ChunkErrorCode generateActiveMesh(std::shared_ptr<ActiveChunk>& activeChunk);

    ChunkErrorCode generateVertices(ChunkData* chunkData, std::vector<TextureVertex>& vertices);

    static void getRegionCoordsFromChunkCoords(int chunk_x, int chunk_z, int* region_x, int* region_z);
    static void getChunkCoordsFromWorldCoords(int world_x, int world_z, int* chunk_x, int* chunk_z);
    void getRegionCoordsFromWorldCoords(int world_x, int world_z, int* region_x, int* region_z);

    void updateMeshes();
    void render(Shader* shader);

    void setWorldPos(glm::vec3 worldPos);

    void update();
    void cleanup();
};