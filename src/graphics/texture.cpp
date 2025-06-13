//
// Created by yashas on 6/12/25.
//

#include "texture.h"

#include <libintl.h>

Texture::Texture(const char* texturePath) {
	setup(texturePath);
}

Texture::~Texture() = default;

void Texture::setup(const char *texturePath) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	void *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	else {
		std::cerr << "Error in loading asset" << std::endl;
	}

	stbi_image_free(data);
}

bool Texture::activate(unsigned int unitIndex){
	// turn an index (0,1,2…) into the correct enum:
	GLenum unitEnum = GL_TEXTURE0 + unitIndex;
	glActiveTexture(unitEnum);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "Failed to activate texture unit " << unitIndex
				  << " (enum " << std::hex << unitEnum << std::dec
				  << ") with error: " << std::hex << err << std::dec << "\n";
		return false;
	}

	// now bind the texture into that slot
	glBindTexture(GL_TEXTURE_2D, id);
	texture_unit = unitIndex;
	return true;
}


unsigned int Texture::getId() const {
	return id;
}

unsigned int Texture::getUnit() const {
	return texture_unit;
}

void Texture::cleanup() {
	if (id) {
		glDeleteTextures(1, &id);
		id = 0;
	}
}


