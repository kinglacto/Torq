#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

class Texture {
private:
	unsigned int id{0};
	unsigned int texture_unit{};
	void setup(const std::string& texturePath);
public:
	explicit Texture(const std::string& texturePath);
	~Texture();
	bool activate(unsigned int unit);
	[[nodiscard]] unsigned int getId() const;
	[[nodiscard]] unsigned int getUnit() const;
	void cleanup();
};



#endif //TEXTURE_H
