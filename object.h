#pragma once

#include <glad/glad.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"

class object {
    private:
    GLuint VBO, VAO;
    GLuint nVertices;
    shaderProgram& shader;
    GLuint texture;

    void texParams();

    public:
    object(GLfloat* vertices, size_t nAttribs, GLuint nVerts, shaderProgram& shaderProg);
    void loadTexture(std::string name);
    //object() = delete;
    void draw();
};