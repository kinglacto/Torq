#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include "chunk.h"
#include "shader.h"
#include <vector>
#include "vertex.h"
#include <memory>

class ChunkMesh{
public:
    int chunk_x, chunk_z;
    GLuint VAO, VBO;
    bool isActive;
    bool chunkEverUpdated;
    int vertices_size;
    std::unique_ptr<Chunk> chunk;

    ChunkMesh(int chunk_x, int chunk_z);
    ChunkMesh(int chunk_x, int chunk_z, std::unique_ptr<Chunk> chunk);
    ~ChunkMesh();
    
    void generateMesh(Chunk* localChunk=nullptr);
    void deleteMesh();

    static void generateVertices(std::vector<TextureVertex>& vertices, Chunk* chunk);
    void render(Shader* shader);
};

#endif