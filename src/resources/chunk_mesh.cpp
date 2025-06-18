#include "chunk_mesh.h"

ChunkMesh::ChunkMesh(int chunk_x, int chunk_z): VAO{0}, VBO{0}, isActive{false}, chunkEverUpdated{false},
chunk_x{chunk_x},chunk_z{chunk_z}, vertices_size{0}, chunk{nullptr} {

}

ChunkMesh::ChunkMesh(int chunk_x, int chunk_z, std::unique_ptr<Chunk> chunk): VAO{0}, VBO{0}, isActive{false}, chunk_x{chunk_x},
chunkEverUpdated{false}, chunk_z{chunk_z}, vertices_size{0}, chunk{std::move(chunk)} {

}

ChunkMesh::~ChunkMesh(){
    deleteMesh();
}

void ChunkMesh::generateMesh(Chunk* localChunk=nullptr){
    if (localChunk == nullptr){
        if (chunk != nullptr){
            localChunk = chunk.get();
        }

        else{
            return;
        }
    }

    if (VAO != 0){
        glDeleteVertexArrays(1, &VAO);
		VAO = 0;
    }

    std::vector<TextureVertex> vertices;

    ChunkMesh::generateVertices(vertices, localChunk);
    vertices_size = vertices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextureVertex), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
    	sizeof(TextureVertex), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(TextureVertex), reinterpret_cast<void *>(offsetof(TextureVertex, normal)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		sizeof(TextureVertex), reinterpret_cast<void *>(offsetof(TextureVertex, tex)));
	glEnableVertexAttribArray(2);

}

void ChunkMesh::deleteMesh(){
    if (VAO) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

    if (VBO) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
}

void ChunkMesh::render(Shader* shader){
    if (vertices_size == 0) return;
    glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices_size);
	glBindVertexArray(0);
}

void ChunkMesh::generateVertices(std::vector<TextureVertex>& vertices, Chunk* chunk){
    
    return;
}