#ifndef SPHERE_H
#define SPHERE_H

#include "model.h"
#include "vertex.h"

class Sphere: public PrimitiveModel {
	glm::vec3 scale{100.0f};
	float radius{100.0f};
	glm::vec3 position{glm::vec3(0.0f, 0.0f, 0.0f)};
	glm::vec3 curr{};

	Shader* shader{nullptr};
	Texture* texture{nullptr};

	std::vector<PrimitiveVertex> vertices;
	std::vector<unsigned int> indices;
public:
	glm::mat4 model{glm::mat4(1.0f)};

	Sphere(glm::vec3 pos, float radius);
	Sphere(glm::vec3 pos, float radius, Shader* shader, Texture* texture);
	~Sphere();

	void init();
	void render();

	bool setRadius(float radius);
	void setPosition(glm::vec3 pos);
	void setShader(Shader* shader);
	void setTexture(Texture* texture);
};



#endif //SPHERE_H
