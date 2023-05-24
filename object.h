/*
   File: object.h
   Description: Objects in 3D space.
   Author: Richard Váňa
   Date: May 23, 2023
*/

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

    // Enumeration for object types
    enum TYPE {
        UNDEFINED = 0,
        NONE = 3,
        TEXTURED = 8,
        COLORED = 9,
        TEXTURED_COLORED = 11
    };

    // Enumeration for texture types
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

    // Setter for the 'explode' variable
    void setExplode(float e) { explode = e;};

    // Setter for custom axis of rotation
    void setCustomAxis(float x, float y, float z);

    // Setter for position of the custom axis of rotation 
    void setCustomPos(float x, float y, float z);

    // roate around the custom axis
    void customRotate(float deg);

    // Attach a child object to the current object
    void attachChild(object* kid);

    // constructors
    object() = delete;
    object(GLuint nAttribs, GLuint nVerts, GLfloat* vertices, GLuint nIndcs, GLuint* indices, shaderProgram& shaderProg);
    object(GLuint nAttribs, GLfloat* vertices, GLuint nVerts, shaderProgram& shaderProg);

    // Refill the vertex buffer with new data
    bool refillBuffers(GLuint nAttribs, GLfloat* buffer);

    // loads and sets texture for object
    void loadTexture(std::string name, TEXTURE_TYPE texType, int texNo);


    void draw();

    // update model setting model matrice to eye
    void updateModel();
    
    // update model using previous model matrix
    void relUpdateModel();
    
    

    void scale(float x, float y, float z);
    void setPos(glm::vec3 pos) {
        position = pos;
        relUpdateModel();
    };

    //translate object in own direction
    void ttranslate(float delta) {
        model = glm::translate(model, delta * glm::vec3(0, 1.0f, 0));
        translate(trackFwdDir * delta);

        for (object *kid : childObjects) {
            kid->ttranslate(delta);
        }
    };


    void setDir(glm::vec3 dir);

    // quaternion rotation
    void rotateQ(glm::quat quat);

    // rotate relative
    void rotateErel(float xDeg, float yDeg, float zDeg);

    // rotate absolute
    void rotateEabs(float xDeg, float yDeg, float zDeg);
    
    // get object direction    
    glm::vec3 getNavDir();


    // rotation
    void pitch(float deg);
    void yaw(float deg);
    void roll(float deg);

    // set material properties
    void setMtl(float* mtls);
};