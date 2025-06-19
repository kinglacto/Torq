#include <chunk_mesh.h>

ChunkMesh::ChunkMesh(int chunk_x, int chunk_z): chunk_x{chunk_x}, chunk_z{chunk_z}, VAO{0}, VBO{0},
vertexCount{0}{

};

ChunkMesh::~ChunkMesh(){
    deleteMesh();
}

ChunkErrorCode ChunkMesh::uploadMesh(std::vector<TextureVertex>& vertices){
    deleteMesh();
    vertexCount = vertices.size();

    if (vertexCount == 0){
        return NO_ERROR;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(TextureVertex), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(TextureVertex), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(TextureVertex), reinterpret_cast<void *>(offsetof(TextureVertex, normal)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        sizeof(TextureVertex), reinterpret_cast<void *>(offsetof(TextureVertex, tex)));
    glEnableVertexAttribArray(2);
    return NO_ERROR;
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

ChunkErrorCode ChunkMesh::render(Shader* shader){
    if (vertexCount == 0) return CHUNK_MESH_CORRUPTED;
    glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
    return NO_ERROR;
}
