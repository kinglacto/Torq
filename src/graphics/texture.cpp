//
// Created by yashas on 6/12/25.
//

#include "texture.h"

#include <libintl.h>

Texture::Texture(const std::string& texturePath) {
	setup(texturePath);
}

Texture::~Texture() = default;

void Texture::setup(const std::string& texturePath) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// To-Do: Add functions that allow the user to set these parameters

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	void *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {

		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		else {
			std::cerr << "Unsupported number of channels: " << nrChannels << std::endl;
			stbi_image_free(data);
			return;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	else {
		std::cerr << "Error in loading asset" << std::endl;
	}

	stbi_image_free(data);
}

bool Texture::activate(unsigned int unitIndex){
	GLenum unitEnum = GL_TEXTURE0 + unitIndex;
	glActiveTexture(unitEnum);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "Failed to activate texture unit " << unitIndex
				  << " (enum " << std::hex << unitEnum << std::dec
				  << ") with error: " << std::hex << err << std::dec << "\n";
		return false;
	}

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


