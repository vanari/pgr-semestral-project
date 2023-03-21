#include "shader.h"

shaderProgram::shaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch(std::ifstream::failure e) {
        std::cerr << "Error: could not read shader file!" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader, fragmentShader;

    GLint success;
    char infoLog[512];

    // vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Error: vertex shader compilation failed!" << std::endl << infoLog << std::endl;
    }

    // fragment shader    
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Error: fragment shader compilation failed!" << std::endl << infoLog << std::endl;
    }

    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Error: shader program linking failed!" << std::endl << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
};

void shaderProgram::use() {
    glUseProgram(ID);
}

/*template <typename T>
void shaderProgram::setUniform(const std::string& name, T&& value) const {
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

}*/