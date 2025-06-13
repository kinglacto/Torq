#include "shader.h"

Shader::Shader() = default;

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    std::cerr << "starting shader compilation" << std::endl;
    generate(vertexShaderPath, fragmentShaderPath);
}

Shader::~Shader() = default;

void Shader::generate(const char* vertexShaderPath, const char* fragmentShaderPath) {
    int success;

    const GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
    const GLuint fragmentShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (!success) {
        char logInfo[512];
        glGetProgramInfoLog(id, 512, nullptr, logInfo);
        std::cerr << "Linking error with program: " << std::endl << logInfo << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GLuint Shader::compileShader(const char* filepath, GLenum type) {
    int success;
    const GLuint Shader_id = glCreateShader(type);
    const GLchar* Shader_src = loadShaderSrc(filepath);

    if (!Shader_src) {
        std::cerr << "Failed to load the shader source" << std::endl;
    }

    glShaderSource(Shader_id, 1, &Shader_src, nullptr);
    glCompileShader(Shader_id);
    glGetShaderiv(Shader_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        char logInfo[512];
        glGetShaderInfoLog(Shader_id, 512, nullptr, logInfo);
        std::cerr << "Error with vertex shader compilation: " << std::endl << logInfo << std::endl;
    }
    free((void*) Shader_src);
    return Shader_id;
}

const char* Shader::loadShaderSrc(const char* filepath) {
    FILE* sp = fopen(filepath, "r");
    if (sp == nullptr) {
        std::cout << "Failed to open file " << filepath << std::endl;
        return nullptr;
    }

    fseek(sp, 0, SEEK_END);
    long int length = ftell(sp);
    fseek(sp, 0, SEEK_SET);

    char* buffer = static_cast<char*>(malloc(length + 1));
    fread(buffer, 1, length, sp);
    buffer[length] = '\0';

    fclose(sp);
    return buffer;
}

void Shader::setMat4(const std::string& name, glm::mat4& val) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::activate() const {
    glUseProgram(id);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

unsigned int Shader::getId() const {
    return id;
}

void Shader::cleanup() {
	if (id) {
		glDeleteProgram(id);
		id = 0;
	}
}
