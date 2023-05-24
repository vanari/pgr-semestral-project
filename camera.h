/*
   File: camera.h
   Description: Camera object.
   Author: Richard Váňa
   Date: May 23, 2023
*/

#pragma once

#include "generalObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class camera : public generalObject {
    private:
    glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    
    float fov = glm::radians(45.0f);
    float ratio = 1.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix = glm::mat4(1.0f);

    shaderProgram **shaderList = nullptr;
    unsigned int nShaders = 0;


    public:
    camera(unsigned int numShaders, shaderProgram** sList);

    // sets projection and view matrices in uniform
    void updateShaders();


    void setDir(glm::vec3 dir);

    // sets the direction that is considerede Up in the camera view
    void setUpDir(glm::vec3 upDir);

    // sets screen ration (width/height)
    void setRatio(float ratio);

    // updates the projection and view matrices.
    void calculate();
};