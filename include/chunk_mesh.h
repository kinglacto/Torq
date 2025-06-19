#pragma once

#include "chunk_utility.h"
#include "vertex.h"
#include <vector>
#include <glad/glad.h>
#include "shader.h"

class ChunkMesh{
public:
    GLuint VAO, VBO;
    int vertexCount;
    int chunk_x, chunk_z;

    ChunkMesh(int chunk_x, int chunk_z);
    ~ChunkMesh();

    ChunkErrorCode uploadMesh(std::vector<TextureVertex>& vertices);
    ChunkErrorCode render(Shader* shader);
    void deleteMesh();
};