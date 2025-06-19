#pragma once

#include <model.h>
#include <vertex.h>
#include <texture.h>
#include <shader.h>

class Cube: public TextureModel {
protected:
	glm::vec3 size;

	Shader* shader;
	Texture* texture;

	std::vector<TextureVertex> vertices;
	std::vector<unsigned int> indices;

public:
	glm::vec3 position;
	glm::mat4 model;
	Cube(glm::vec3 pos, glm::vec3 size, Shader* shader, Texture* texture);
	Cube(glm::vec3 pos, glm::vec3 size);

	Cube(glm::vec3 pos, float size, Shader* shader, Texture* texture);
	Cube(glm::vec3 pos, float size);

	~Cube();
	
	void init();
	void render();
	
	bool setSize(float size);
	bool setSize(glm::vec3 size);
	void setPosition(glm::vec3 pos);
	void setShader(Shader* shader);
	void setTexture(Texture* texture);
};
