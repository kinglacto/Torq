#ifndef MESH_H
#define MESH_H

#include <vector>
#include "shader.h"
#include "texture.h"
#include "vertex.h"

class PrimitiveMesh {
private:
	bool changed_element_indices {false};
public:
	bool elementDraw {false};
	std::vector<PrimitiveVertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO{0}, VBO{0}, EBO{0};
	PrimitiveMesh() = default;
	explicit PrimitiveMesh(std::vector<PrimitiveVertex> vertices);
	PrimitiveMesh(std::vector<PrimitiveVertex> vertices, std::vector<unsigned int> indices);

	~PrimitiveMesh() = default;

	void render() const;
	void cleanup();
	void setup();
	void setVertices(std::vector<PrimitiveVertex> vertices);
	void setIndices(std::vector<unsigned int> indices);
	void setElementDraw(bool flag);
};

class TextureMesh {
private:
	bool changed_element_indices {false};
public:
	bool elementDraw {false};
	std::vector<TextureVertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO{0}, VBO{0}, EBO{0};
	TextureMesh() = default;
	explicit TextureMesh(std::vector<TextureVertex> vertices);
	TextureMesh(std::vector<TextureVertex> vertices, std::vector<unsigned int> indices);

	~TextureMesh() = default;

	void render() const;
	void cleanup();
	void setup();
	void setVertices(std::vector<TextureVertex> vertices);
	void setIndices(std::vector<unsigned int> indices);
	void setElementDraw(bool flag);
};

#endif //MESH_H
