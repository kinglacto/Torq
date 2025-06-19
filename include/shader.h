#pragma once

#include <glad/glad.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
	unsigned int id{};
	void generate(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	static const char* loadShaderSrc(const std::string& filepath);
	static GLuint compileShader(const std::string& filepath, GLenum type);
public:
	Shader();
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();
	void activate() const;

	void cleanup();

	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4& val) const;
	void set3Float(const std::string& name, glm::vec3 v) const;
	void set3Float(const std::string& name, float v1, float v2, float v3) const;

	[[nodiscard]] unsigned int getId() const;
};
