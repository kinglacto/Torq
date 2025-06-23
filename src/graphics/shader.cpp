#include "shader.h"

#include "assets.h"

Shader::Shader() = default;

Shader::Shader(const std::string& vertexShaderPath, const std::string&  fragmentShaderPath) {
    generate(vertexShaderPath, fragmentShaderPath);
}

Shader::~Shader() = default;

void Shader::generate(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
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

GLuint Shader::compileShader(const std::string& filepath, GLenum type) {
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

    delete[] Shader_src;
    return Shader_id;
}


const char* Shader::loadShaderSrc(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filepath << std::endl;
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[size + 1];
    if (!file.read(buffer, size)) {
        std::cerr << "Failed to read file " << filepath << std::endl;
        delete[] buffer;
        return nullptr;
    }

    buffer[size] = '\0'; 
    return buffer;
}

void Shader::setMat4(const std::string& name, glm::mat4& val) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::activate() const {
    glUseProgram(id);
}

void Shader::setInt(const std::string& name, int value) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform1i(loc, value);
    else std::cerr << "Error setting uniform variable " << name << ", could not locate it" << std::endl;
}

void Shader::setFloat(const std::string& name, float value) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform1f(loc, value);
    else std::cerr << "Error setting uniform variable " << name << ", could not locate it" << std::endl;
    
}

void Shader::set3Float(const std::string& name, glm::vec3 v) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform3fv(loc, 1, glm::value_ptr(v));
    else std::cerr << "Error setting uniform variable " << name << ", could not locate it" << std::endl;
}   

void Shader::set3Float(const std::string& name, float v1, float v2, float v3) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform3f(loc, v1, v2, v3);
    else std::cerr << "Error setting uniform variable " << name << ", could not locate it" << std::endl;
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
