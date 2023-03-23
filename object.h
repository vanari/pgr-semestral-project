#pragma once

#include <glad/glad.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"

#include <glm/glm.hpp>

#define VERT_LOC 0
#define NORM_LOC 1
#define CLR_LOC  2
#define TEX_LOC  3

class object {
    public:
    /*
        location 0: vertices        3f
        location 1: normals         3f
        location 2: colors          3f
        location 3: teture coords   2f

    */
    enum TYPE {
        UNDEFINED = 0,
        NONE = 3,
        TEXTURED = 8,
        COLORED = 9,
        TEXTURED_COLORED = 11
    };

    private:
    bool hasEBO = false;
    TYPE type = NONE;
    GLuint VBO, VAO, EBO;
    GLuint nVertices;
    shaderProgram& shader;
    GLuint texture;
    glm::vec3 defaultColor = glm::vec3(0.5f, 0.5f, 0.5f);

    void texParams();
    bool fillBuffers(size_t nAttribs, GLfloat* buffer, GLuint* indices);

    public:
    object() = delete;
    object(GLfloat* vertices, GLuint* indices, size_t nAttribs, GLuint nVerts, shaderProgram& shaderProg);
    void loadTexture(std::string name);
    void draw();
};