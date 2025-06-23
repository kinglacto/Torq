#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include "texture_utility.h"

struct Tex{
	TexMap id;
	int width, height, nrChannels;
	unsigned char* data{nullptr};
};

class Texture {
private:
	int tileSize;
	int tilesPerRow;
	int atlasSize;

	int width, height, nrChannels;
public:
	std::map<TexMap, glm::vec4> uvMap;

private:
	std::vector<Tex> textures;
	unsigned int id;
	unsigned int texture_unit;
	void setup(const std::string& texturePath);
	void loadTextures(const std::string& texturePath);
public:
	Texture(const std::string& texturePath);
	~Texture();
	bool activateAt(unsigned int unit);
	[[nodiscard]] unsigned int getId() const;
	[[nodiscard]] unsigned int getUnit() const;
	void cleanup();

	void set_wrap_s(GLint param);
	void set_wrap_t(GLint param);
	void set_mag_filter(GLint param);
	void set_min_filter(GLint param);
};
