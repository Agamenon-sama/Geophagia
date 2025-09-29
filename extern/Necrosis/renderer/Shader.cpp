#include "Shader.h"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>

namespace Necrosis {

std::shared_ptr<Shader> Shader::makeFromString(const std::string &source) {
    _log.setPrefix("Error::Shader: ");

    std::string vertexSource, fragmentSource;
    std::stringstream sourceStream(source);
    std::string line;
    int lineNumber = 0;

    // read the source code
    // 0 = vertex, 1 = fragment
    int parseState = -1;

    while (std::getline(sourceStream, line)) {
        lineNumber++;
        if (line == "#vertex") {
            parseState = 0;
            vertexSource += "#version 450 core\n";
            vertexSource += "uniform mat4 u_model;\n";
            vertexSource += "uniform mat4 u_view;\n";
            vertexSource += "uniform mat4 u_projection;\n";
            vertexSource += "#line " + std::to_string(lineNumber+1) + "\n";
            continue;
        }
        else if (line == "#fragment") {
            parseState = 1;
            fragmentSource += "#version 450 core\n";
            fragmentSource += "uniform vec2 u_resolution;\n";
            fragmentSource += "uniform float u_time;\n";
            fragmentSource += "#line " + std::to_string(lineNumber+1) + "\n";
            continue;
        }
        else if (parseState == -1) {
            slog::warning("Shader: No shader type specified. Proceeding with #vertex");
            parseState = 0;
        }

        if (parseState == 0) vertexSource += line + "\n";
        if (parseState == 1) fragmentSource += line + "\n";
    }

    // Building
    const char *vertexCSource = vertexSource.c_str();
    const char *fragmentCSource = fragmentSource.c_str();
    uint32_t vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCSource, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        _log("Failed to compile vertex shader\n" + std::string(infoLog));
        return nullptr;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCSource, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        _log("Failed to compile fragment shader\n" + std::string(infoLog));
        return nullptr;
    }

    // Linking
    uint32_t id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        _log("Failed to link shader\n" + std::string(infoLog));
        return nullptr;
    }

    glDetachShader(id, vertex);
    glDeleteShader(vertex);
    glDetachShader(id, fragment);
    glDeleteShader(fragment);

    return std::shared_ptr<Shader>(new Shader(id));
}

std::shared_ptr<Shader> Shader::makeFromFile(const std::string &path) {
    _log.setPrefix("Error::Shader: ");

    std::fstream file(path, std::ios::in);
    if (!file.is_open()) {
        _log("Can't open file " + path);
        return nullptr;
    }

    std::stringstream ss;
    ss << file.rdbuf();

    return makeFromString(ss.str());
}

Shader::Shader(uint32_t id) : _id(id) {}

Shader::~Shader() {
    glDeleteProgram(_id);
}


void Shader::use() const{
    glUseProgram(_id);
}

void Shader::setBool(const std::string &name, bool value) const {
    use();
    glUniform1i(glGetUniformLocation(_id, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    use();
    glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    use();
    glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
}

void Shader::setVec3f(const std::string &name, glm::vec3 value) const {
    use();
    glUniform3f(glGetUniformLocation(_id, name.c_str()), value[0], value[1], value[2]);
}

void Shader::setMat4f(const std::string &name, glm::mat4 value) const {
    use();
    glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &value[0][0]); // glm::value_ptr is also possible
}

}
