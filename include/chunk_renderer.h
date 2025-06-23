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
#include <glm/glm.hpp>

class ChunkRenderer{
public:
    glm::vec3 worldPos;
    Texture* texture;
    glm::vec3 block_scaling_factor{10};

    std::map<std::pair<int, int>, std::unique_ptr<ChunkMesh>> chunkMeshes;
    std::map<std::pair<int, int>, std::unique_ptr<ActiveChunk>> activeChunks;
    std::shared_ptr<ChunkLoader> chunkLoader;

    std::queue<std::pair<ChunkMesh*, std::unique_ptr<ChunkData>>> ChunkMeshQueue;
    std::queue<ActiveChunk*> ActiveChunkRemeshQueue;

    ChunkErrorCode deleteActiveChunk(const std::pair<int, int>& chunk_coords);
    ChunkErrorCode deleteChunkMesh(const std::pair<int, int>& chunk_coords);

    ChunkErrorCode makeChunkMesh(const std::pair<int, int>& chunk_coords);
    ChunkErrorCode makeChunkActive(const std::pair<int, int>& chunk_coords);

    ChunkErrorCode generateStaticMesh(std::pair<ChunkMesh*, std::unique_ptr<ChunkData>>& it);
    ChunkErrorCode generateActiveMesh(ActiveChunk* activeChunk);

    ChunkErrorCode generateVertices(ChunkData* chunkData, std::vector<TextureVertex>& vertices);

    void getRegionCoordsFromChunkCoords(int chunk_x, int chunk_z, int* region_x, int* region_z);
    void getChunkCoordsFromWorldCoords(int world_x, int world_z, int* chunk_x, int* chunk_z);
    void getRegionCoordsFromWorldCoords(int world_x, int world_z, int* region_x, int* region_z);

    void updateMeshes();
    void render(Shader* shader);

    void update(Shader* shader);
    void cleanup();
};