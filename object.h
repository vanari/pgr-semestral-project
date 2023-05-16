#pragma once

#include "generalObject.h"

#include <glad/glad.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"

#include <glm/glm.hpp>

#define VERT_LOC 0
#define NORM_LOC 1
#define CLR_LOC  2
#define TEX_LOC  3

class object : public generalObject{
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

    enum TEXTURE_TYPE {
        RGB,
        RGBA
    };

    private:
    bool hasEBO = false;
    TYPE type = NONE;
    GLuint VBO, VAO, EBO = 0;
    GLuint nVertices;
    GLuint nIndices;
    shaderProgram& shader;
    GLuint texture;
    glm::vec3 defaultColor = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 model = glm::mat4(1.0f);
    GLfloat xAngle = 1.0f;
    glm::vec3 xDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 scaleVector = glm::vec3(1.0f);

    void texParams();
    bool fillBuffers(GLuint nAttribs, GLfloat* buffer, GLuint* indices);

    public:
    object() = delete;
    object(GLuint nAttribs, GLuint nVerts, GLfloat* vertices, GLuint nIndcs, GLuint* indices, shaderProgram& shaderProg);
    object(GLuint nAttribs, GLfloat* vertices, GLuint nVerts, shaderProgram& shaderProg);
    bool refillBuffers(GLuint nAttribs, GLfloat* buffer);
    void loadTexture(std::string name, TEXTURE_TYPE texType);
    void draw();
    void updateModel();
    void scale(float x, float y, float z);
    void setPos(glm::vec3 pos) {
        position = pos; //generalObject::setPos(pos);
        updateModel();
    };
    void setDir(glm::vec3 dir);
    void rotateQ(glm::quat quat);
    void rotateErel(float xDeg, float yDeg, float zDeg);
    void rotateEabs(float xDeg, float yDeg, float zDeg);
};