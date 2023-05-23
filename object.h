#pragma once

#include "generalObject.h"

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include "shader.h"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#define VERT_LOC 0
#define NORM_LOC 1
#define CLR_LOC  2
#define TEX_LOC  3

class object : public generalObject{
    public:

    float screenRatio = 1.0f;

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
    float explode = 0.0f;
    bool osc = false;
    GLfloat material[3*3+1];
    bool hasEBO = false;
    TYPE type = NONE;
    GLuint VBO, VAO, EBO = 0;
    GLuint nVertices;
    GLuint nIndices;
    shaderProgram& shader;
    GLuint textures[4] = {0,0,0,0};
    glm::vec3 defaultColor = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 model = glm::mat4(1.0f);
    GLfloat xAngle = 1.0f;
    glm::vec3 xDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 scaleVector = glm::vec3(1.0f);
    std::vector<object*> childObjects;

    glm::vec3 customAxis = glm::vec3(0, 0, 1.0f);
    glm::vec3 customPos = glm::vec3(0, 0, 1.0f);

    glm::vec3 navUpDir = glm::vec3(0, 0, 1.0f);
    glm::vec3 navFwdDir = glm::vec3(0, 1.0f, 0);

    glm::vec3 trackUpDir = glm::vec3(0, 0, 1.0f);
    glm::vec3 trackFwdDir = glm::vec3(0, 1.0f, 0);

    void texParams();
    bool fillBuffers(GLuint nAttribs, GLfloat* buffer, GLuint* indices);

    public:
    void setExplode(float e) { explode = e;};
    void setCustomAxis(float x, float y, float z);
    void setCustomPos(float x, float y, float z);
    void customRotate(float deg);
    void attachChild(object* kid);
    object() = delete;
    object(GLuint nAttribs, GLuint nVerts, GLfloat* vertices, GLuint nIndcs, GLuint* indices, shaderProgram& shaderProg);
    object(GLuint nAttribs, GLfloat* vertices, GLuint nVerts, shaderProgram& shaderProg);
    bool refillBuffers(GLuint nAttribs, GLfloat* buffer);
    void loadTexture(std::string name, TEXTURE_TYPE texType, int texNo);
    void draw();
    void updateModel();
    void relUpdateModel();
    void scale(float x, float y, float z);
    void setPos(glm::vec3 pos) {
        position = pos; //generalObject::setPos(pos);
        relUpdateModel();
    };
    void ttranslate(float delta) {
        model = glm::translate(model, delta * glm::vec3(0, 1.0f, 0));
        translate(trackFwdDir * delta);

        for (object *kid : childObjects) {
            kid->ttranslate(delta);
        }
    };
    void setDir(glm::vec3 dir);
    void rotateQ(glm::quat quat);
    void rotateErel(float xDeg, float yDeg, float zDeg);
    void rotateEabs(float xDeg, float yDeg, float zDeg);
    glm::vec3 getNavDir();
    void pitch(float deg);
    void yaw(float deg);
    void roll(float deg);
    void setMtl(float* mtls);
};