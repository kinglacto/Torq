#ifndef SPHERE_H
#define SPHERE_H

#include <model.h>
#include <vertex.h>
#include <material.h>

class Sphere: public PrimitiveModel {
protected:
	glm::vec3 scale{100.0f};
	float radius{100.0f};
	glm::vec3 curr{};

	Shader* shader{nullptr};
	Texture* texture{nullptr};

	std::vector<PrimitiveVertex> vertices;
	std::vector<unsigned int> indices;

	Material material;
	bool using_material{false};
public:
	glm::vec3 position{glm::vec3(0.0f, 0.0f, 0.0f)};
	glm::mat4 model{glm::mat4(1.0f)};

	Sphere(glm::vec3 pos, float radius);
	Sphere(glm::vec3 pos, float radius, Shader* shader, Texture* texture);

	Sphere(glm::vec3 pos, float radius, const Material& material);
	Sphere(glm::vec3 pos, float radius, Shader* shader, Texture* texture, const Material& material);
	~Sphere();

	void init();
	void render();

	bool setRadius(float radius);
	void setPosition(glm::vec3 pos);
	void setShader(Shader* shader);
	void setTexture(Texture* texture);

	void setMaterial(const Material& material);
	void useMaterial(bool flag);
};



#endif //SPHERE_H
