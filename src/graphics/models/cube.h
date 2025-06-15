#ifndef CUBE_H
#define CUBE_H

#include <model.h>
#include <vertex.h>
#include <texture.h>
#include <shader.h>
#include <material.h>

class Cube: public TextureModel {
protected:
	glm::vec3 size{glm::vec3(100.0f, 100.0f, 100.0f)};
	glm::vec3 curr{};

	Shader* shader{nullptr};
	Texture* texture{nullptr};

	std::vector<TextureVertex> vertices;
	std::vector<unsigned int> indices;

	Material material;
	bool using_material {false};

public:
	glm::vec3 position{glm::vec3(0.0f, 0.0f, 0.0f)};
	glm::mat4 model{glm::mat4(1.0f)};
	Cube(glm::vec3 pos, glm::vec3 size, Shader* shader, Texture* texture);
	Cube(glm::vec3 pos, glm::vec3 size);

	Cube(glm::vec3 pos, glm::vec3 size, Shader* shader, Texture* texture, const Material& material);
	Cube(glm::vec3 pos, glm::vec3 size, const Material& material);

	Cube(glm::vec3 pos, float size, Shader* shader, Texture* texture);
	Cube(glm::vec3 pos, float size);

	Cube(glm::vec3 pos, float size, Shader* shader, Texture* texture, const Material& material);
	Cube(glm::vec3 pos, float size, const Material& material);
	~Cube();
	
	void init();
	void render();
	
	bool setSize(float size);
	bool setSize(glm::vec3 size);
	void setPosition(glm::vec3 pos);
	void setShader(Shader* shader);
	void setTexture(Texture* texture);

	void setMaterial(const Material& material);
	void useMaterial(bool flag);
};



#endif //CUBE_H
