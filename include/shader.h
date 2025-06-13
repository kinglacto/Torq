#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <cstdlib>
#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
	unsigned int id{};
public:
	Shader();
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	~Shader();
	void activate() const;

	void cleanup();

	void generate(const char* vertexShaderPath, const char* fragmentShaderPath);
	static const char* loadShaderSrc(const char* filepath);
	static GLuint compileShader(const char* filepath, GLenum type);

	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4& val) const;

	[[nodiscard]] unsigned int getId() const;
};

#endif //SHADER_H
