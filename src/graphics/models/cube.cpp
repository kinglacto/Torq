#include "cube.h"
#include <iostream>

Cube::Cube(const glm::vec3 pos, const float length, Shader* shader, Texture* texture):
	length(length), position(pos), shader(shader), texture(texture) {
	curr = position;
	setLength(length);
}

Cube::Cube(const glm::vec3 pos, const float length) :
	length(length), position(pos){
	curr = position;
	setLength(length);
}

Cube::~Cube() = default;

void Cube::init() {
	model = glm::mat4(1.0f);
	int noVertices = 36;

    float v[] = {
        // position                 color              texcoord
        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, 1.0f,    //0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,     0.0f,  0.0f, 1.0f,    //1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  0.0f, 1.0f,    //1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  0.0f, 1.0f,    //1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,     0.0f,  0.0f, 1.0f,    //0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, 1.0f,    //0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   // 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   // 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   // 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   // 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   // 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,   // 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    //1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    //1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    //0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    //0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    //0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    //1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,   // 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,   //1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,   //0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,   //0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,   //0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,   //1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f,  0.0f,    //0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     0.0f, 1.0f,  0.0f,    //1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     0.0f, 1.0f,  0.0f,    //1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     0.0f, 1.0f,  0.0f,    //1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,     0.0f, 1.0f,  0.0f,    //0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f,  0.0f,    //0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,    //0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,    //1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,    //1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,    //1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,    //0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,    //0.0f, 1.0f
    };

	vertices.resize(noVertices);
	for (int i = 0; i < noVertices; i++) {
		int base = i * 6;
		vertices[i].pos   = glm::vec3(v[base], v[base+1], v[base+2]);
		vertices[i].color = glm::vec3(v[base+3], v[base+4], v[base+5]);
	}
	meshes.push_back(PrimitiveMesh(vertices));
}


void Cube::render() {
	if (!shader) {
		std::cerr << "Failed to render the cube, shader is NULL" << std::endl;
		return;
	}

	model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
	shader->setMat4("model", model);
	renderAll();
}

void Cube::setShader(Shader *shader){
	this->shader = shader;
}

void Cube::setTexture(Texture *texture){
	this->texture = texture;
}

bool Cube::setLength(const float length) {
	if (length > 0.0f) {
		Cube::length = length;
		scale = glm::vec3(length);
		return true;
	}

	return false;
}

void Cube::setPosition(const glm::vec3 pos) {
	position = pos;
}