#ifndef CUBE_H
#define CUBE_H

#include "model.h"
#include "vertex.h"
#include "texture.h"
#include "shader.h"

class Cube: public TextureModel {
	glm::vec3 scale{100.0f};
	float length{100.0f};
	glm::vec3 position{glm::vec3(0.0f, 0.0f, 0.0f)};
	glm::vec3 curr{};

	Shader* shader{nullptr};
	Texture* texture{nullptr};

	std::vector<TextureVertex> vertices;
	std::vector<unsigned int> indices;

public:
	glm::mat4 model{glm::mat4(1.0f)};
	Cube(glm::vec3 pos, float length, Shader* shader, Texture* texture);
	Cube(glm::vec3 pos, float length);
	~Cube();
	
	void init();
	void render();
	
	bool setLength(float length);
	void setPosition(glm::vec3 pos);
	void setShader(Shader* shader);
	void setTexture(Texture* texture);
};



#endif //CUBE_H
