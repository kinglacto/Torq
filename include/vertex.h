#pragma once
#include <glm/glm.hpp>

struct PrimitiveVertex {
	glm::vec3 pos;
	glm::vec3 color;
};

struct TextureVertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 tex;
};
