//
// Created by yashas on 3/20/25.
//

#include <mesh.h>
#include <utility>

PrimitiveMesh::PrimitiveMesh(std::vector<PrimitiveVertex> vertices)
    : vertices(std::move(vertices)){
    setup();
}

PrimitiveMesh::PrimitiveMesh(std::vector<PrimitiveVertex> vertices,
	std::vector<unsigned int> indices): vertices(std::move(vertices)), indices(std::move(indices)) {
	elementDraw = true;
	setup();
}

void PrimitiveMesh::render() const {
	glBindVertexArray(VAO);
	if (elementDraw) {
		glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT, static_cast<void *>(nullptr));
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}
	glBindVertexArray(0);
}

void PrimitiveMesh::cleanup() {
	if (VAO) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	if (VBO) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (elementDraw) {
		if (EBO) {
			glDeleteBuffers(1, &EBO);
			EBO = 0;
		}
	}
}

void PrimitiveMesh::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PrimitiveVertex), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
    	sizeof(PrimitiveVertex), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(PrimitiveVertex), reinterpret_cast<void *>(offsetof(PrimitiveVertex, color)));
	glEnableVertexAttribArray(1);

	if (elementDraw) {
		glGenBuffers(1, &EBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
			&indices[0], GL_STATIC_DRAW);

	}
}

void PrimitiveMesh::setVertices(std::vector<PrimitiveVertex> vertices) {
	PrimitiveMesh::vertices = std::move(vertices);
}

void PrimitiveMesh::setIndices(std::vector<unsigned int> indices) {
	changed_element_indices = true;
	PrimitiveMesh::indices = std::move(indices);
}

void PrimitiveMesh::setElementDraw(bool flag) {
	if (flag == false) {
		elementDraw = false;
	}

	else {
		if (EBO != 0 && changed_element_indices) {
			changed_element_indices = false;
			glDeleteBuffers(1, &EBO);
			EBO = 0;
			glBindVertexArray(VAO);

			glGenBuffers(1, &EBO);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				&indices[0], GL_STATIC_DRAW);

		}
		elementDraw = true;
	}
}



TextureMesh::TextureMesh(std::vector<TextureVertex> vertices)
    : vertices(std::move(vertices)){
    setup();
}

TextureMesh::TextureMesh(std::vector<TextureVertex> vertices,
	std::vector<unsigned int> indices): vertices(std::move(vertices)), indices(std::move(indices)) {
	elementDraw = true;
	setup();
}

void TextureMesh::render() const {
	glBindVertexArray(VAO);
	if (elementDraw) {
		glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT, static_cast<void *>(nullptr));
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}
	glBindVertexArray(0);
}

void TextureMesh::cleanup() {
	if (VAO) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	if (VBO) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (elementDraw) {
		if (EBO) {
			glDeleteBuffers(1, &EBO);
			EBO = 0;
		}
	}
}

void TextureMesh::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextureVertex), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
    	sizeof(TextureVertex), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(TextureVertex), reinterpret_cast<void *>(offsetof(TextureVertex, color)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		sizeof(TextureVertex), reinterpret_cast<void *>(offsetof(TextureVertex, tex)));
	glEnableVertexAttribArray(2);

	if (elementDraw) {
		glGenBuffers(1, &EBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
			&indices[0], GL_STATIC_DRAW);

	}
}

void TextureMesh::setVertices(std::vector<TextureVertex> vertices) {
	TextureMesh::vertices = std::move(vertices);
}

void TextureMesh::setIndices(std::vector<unsigned int> indices) {
	changed_element_indices = true;
	TextureMesh::indices = std::move(indices);
}

void TextureMesh::setElementDraw(bool flag) {
	if (flag == false) {
		elementDraw = false;
	}

	else {
		if (EBO != 0 && changed_element_indices) {
			changed_element_indices = false;
			glDeleteBuffers(1, &EBO);
			EBO = 0;
			glBindVertexArray(VAO);

			glGenBuffers(1, &EBO);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				&indices[0], GL_STATIC_DRAW);

		}
		elementDraw = true;
	}
}