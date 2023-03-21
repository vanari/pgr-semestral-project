#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class shaderProgram {
    private:
    GLuint ID;

    public:
    shaderProgram(const char* vertexPath, const char* fragmentPath);
    void use();

    template <typename T>
    void setUniform(const std::string& name, T&& value) const {
        if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, int>) {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<GLint>(value));
        } else if constexpr (std::is_same_v<T, float>) {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), static_cast<GLfloat>(value));
        } else if constexpr (std::is_same_v<T, glm::mat4>) {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
        } else if constexpr (std::is_same_v<T, glm::mat3>) {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
        } else {
            throw std::runtime_error("Unsupported type");
        }

    }

    //template <typename T>
    //void setUniform(const std::string& name, T&& value) const;
};