#pragma once

#include <vector>
#include "shader.h"
#include "texture.h"
#include "vertex.h"

class PrimitiveMesh {
private:
	bool elementDraw {false};
	std::vector<PrimitiveVertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO, VBO, EBO;
public:
	PrimitiveMesh() = default;
	explicit PrimitiveMesh(std::vector<PrimitiveVertex>& vertices);
	PrimitiveMesh(std::vector<PrimitiveVertex>& vertices, std::vector<unsigned int>& indices);

	~PrimitiveMesh() = default;

	void render() const;
	void cleanup();
	void setup();
	void setElementDraw(bool flag);
};

class TextureMesh {
private:
	bool elementDraw {false};
	std::vector<TextureVertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO, VBO, EBO;
public:
	TextureMesh() = default;
	explicit TextureMesh(std::vector<TextureVertex>& vertices);
	TextureMesh(std::vector<TextureVertex>& vertices, std::vector<unsigned int>& indices);

	~TextureMesh() = default;

	void render() const;
	void cleanup();
	void setup();
	void setElementDraw(bool flag);
};
