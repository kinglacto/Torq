#include "sphere.h"
#include <iostream>
Sphere::Sphere(const glm::vec3 pos, const float radius):
radius(radius), position(pos) {
	curr = position;
	setRadius(radius);
}

Sphere::Sphere(const glm::vec3 pos, const float radius, const Material& material):
radius(radius), position(pos), material(material) {
	curr = position;
	setRadius(radius);
	using_material = true;
}



Sphere::Sphere(const glm::vec3 pos, const float radius, Shader* shader, Texture* texture) :
	radius(radius), position(pos), shader(shader), texture(texture){
	curr = position;
	setRadius(radius);
}

Sphere::Sphere(const glm::vec3 pos, const float radius, Shader* shader, Texture* texture,
const Material& material) :
	radius(radius), position(pos), shader(shader), texture(texture), material(material){
	curr = position;
	setRadius(radius);
	using_material = true;
}

Sphere::~Sphere() {
	cleanup();
}

void Sphere::init() {
	model = glm::mat4(1.0f);
	constexpr int sectorCount = 31;
	constexpr int stackCount  = 31;
	constexpr auto PI        = glm::pi<float>();

	float x, y, z, xy;                              

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;
	std::vector<float> v;
	for(int i = 0; i <= stackCount; ++i) {
		float R         = 1.0f;
		stackAngle = PI / 2 - i * stackStep;        
		xy = R * cosf(stackAngle);            
		z = R * sinf(stackAngle);              

		for(int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;          

			x = xy * cosf(sectorAngle);           
			y = xy * sinf(sectorAngle);            
			v.push_back(x);
			v.push_back(y);
			v.push_back(z);

			v.push_back(0.36f);
			v.push_back(0.63f);
			v.push_back(0.83f);
		}
	}

	int vertexCount = v.size()/6;
	vertices.resize(vertexCount);
	std::cout << "successfully built a sphere mesh with vertexCount = " << vertexCount << std::endl;
	for(int i = 0; i < vertexCount; ++i) {
		int base = i * 6;
		vertices[i].pos   = glm::normalize(glm::vec3(v[base], v[base+1], v[base+2]));
		vertices[i].color = glm::vec3(v[base+3], v[base+4], v[base+5]);
	}

	int k1, k2;
	for(int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     
		k2 = k1 + sectorCount + 1;    

		for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			if(i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if(i != (stackCount-1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	meshes.push_back(PrimitiveMesh(vertices, indices));
}


void Sphere::render() {
	if (!shader) {
		std::cerr << "Failed to render the cube, shader is NULL" << std::endl;
		return;
	}

	model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
	shader->setMat4("model", model);
	if (using_material){
		shader->set3Float("material.ambient", material.ambient);
		shader->set3Float("material.diffuse", material.diffuse);
		shader->set3Float("material.specular", material.specular);
		shader->setFloat("material.shininess", material.shininess);
	}
	renderAll();
}


void Sphere::setShader(Shader *shader){
	this->shader = shader;
}

void Sphere::setTexture(Texture *texture){
	this->texture = texture;
}

bool Sphere::setRadius(const float radius) {
	if (radius > 0.0f) {
		Sphere::radius = radius;
		scale = glm::vec3(radius);
		return true;
	}

	return false;
}

void Sphere::setPosition(const glm::vec3 pos) {
	position = pos;
}

void Sphere::setMaterial(const Material& material){
	Sphere::material = material;
}

void Sphere::useMaterial(bool flag){
	using_material = flag;
}