#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

class Texture {
	unsigned int id{0};
	unsigned int texture_unit{};
public:
	explicit Texture(const char* texturePath);
	~Texture();
	void setup(const char* texturePath);
	bool activate(unsigned int unit);
	[[nodiscard]] unsigned int getId() const;
	[[nodiscard]] unsigned int getUnit() const;
	void cleanup();
};



#endif //TEXTURE_H
