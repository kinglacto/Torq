//
// Created by yashas on 6/12/25.
//

#include "texture.h"
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <string.h>

#include <stb_image_write.h>

namespace fs = std::filesystem;

Texture::Texture(const std::string& texturePath) {
	setup(texturePath);
}

Texture::~Texture() = default;

void Texture::setup(const std::string& texturePath) {
	// NOTE: stbi_load, glTexImage2D load/read raw data DIFFERENTLY. 
	// look up their expected formats before reading this code
	
	// Consequence: the jk-loop below can be simplified to use 
	// int srcIndex = (j * tileSize + k) * nrChannels; if this is set to true:
	// stbi_set_flip_vertically_on_load(true);
	// I set it to false (default) so that I can fetch data as is (in png format).

	loadTextures(texturePath);

	if (height == width){
		tileSize = width;
	}

	else{
		std::cerr << "Error loading texture, not of square dimensions: (" << height << ", " << width << ")"
		<< std::endl;
		return;
	}

	tilesPerRow = std::ceil(std::sqrt(textures.size()));
	atlasSize = tilesPerRow * tileSize;
	unsigned char* atlas = (unsigned char*) malloc((atlasSize * atlasSize * nrChannels) *
	sizeof(unsigned char));
	memset(atlas, 0, sizeof(atlas));

	for(int i = 0; i < textures.size(); i++){
		int x = (i % tilesPerRow);
		int y = (i / tilesPerRow);	

		float u0 = static_cast<float>(x * tileSize)/atlasSize;
		float v0 = static_cast<float>(y * tileSize)/atlasSize;
		float u1 = static_cast<float>(x * tileSize + tileSize)/atlasSize;
		float v1 = static_cast<float>(y * tileSize + tileSize)/atlasSize;


		uvMap[textures[i].id] = glm::vec4(u0, v0, u1, v1);

		x *= tileSize;
		y *= tileSize;

		for(int j = 0; j < tileSize; j++){
			for(int k = 0; k < tileSize; k++){
				int srcIndex = ((tileSize - 1 - j) * tileSize + k) * nrChannels;
				int atlasIndex = ((y + j) * atlasSize + (x + k)) * nrChannels;

				for(int channel = 0; channel < nrChannels; channel++){
					atlas[atlasIndex + channel] = textures[i].data[srcIndex + channel];
				}
			}
		}
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if (atlas) {

		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		else {
			std::cerr << "Unsupported number of channels: " << nrChannels << std::endl;
			for(auto t: textures){
				stbi_image_free(t.data);
			}
			return;
		}	

		glTexImage2D(GL_TEXTURE_2D, 0, format, atlasSize, atlasSize, 0, format, GL_UNSIGNED_BYTE, atlas);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	else {
		std::cerr << "Error in loading asset" << std::endl;
	}

	stbi_write_png("atlas_output.png", atlasSize, atlasSize, nrChannels, atlas, atlasSize * nrChannels);
	for(auto t: textures){
		stbi_image_free(t.data);
	}
	free(atlas);
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

void Texture::loadTextures(const std::string& texturePath){
	std::vector<fs::path> texturePaths;
    try {
        for (const auto& entry : fs::directory_iterator(texturePath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".png") {
                    texturePaths.push_back(entry.path());
                }

				else{
					std::cerr << "unsupported image type found in the directory: " << 
					entry.path().filename().string() << std::endl;
				}
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return;
    }

    std::sort(texturePaths.begin(), texturePaths.end());

	idType counter = 0;
	bool firstTexture = true;

	for (auto const& entry : texturePaths) {
        std::string path = entry.string();
		std::string name = entry.filename().string();
	
		Tex t;
		t.id = (texMap) counter;
		t.data = stbi_load(path.c_str(), &t.width, &t.height, &t.nrChannels, 0);
		
		// NOTE: Assumes all png's are of the same SQUARE dimension
		// Will require a total overhaul and re-write if each image does not satisfy this

		if (firstTexture) {
			width = t.width;
			height = t.height;
			nrChannels = t.nrChannels;
			if (width != height) {
				std::cerr << "Error: Base texture " << name << " is not square!" << std::endl;
				return;
			}
			firstTexture = false;
		}
			
		else {
			if (t.width != width || t.height != height || t.nrChannels != nrChannels) {
				// NOTE: skipping invalid textures
				std::cerr << "Error: Texture " << name << " has different dimensions or channels!" << std::endl;
				continue; 
			}
		}

		if (!t.data){
			std::cerr << "Error could not load file: " << name << std::endl;
		}

		textures.push_back(t);
		counter++;
    }
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

void Texture::set_wrap_s(GLint param){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
}

void Texture::set_wrap_t(GLint param){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
}

void Texture::set_mag_filter(GLint param){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
}

void Texture::set_min_filter(GLint param){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
}