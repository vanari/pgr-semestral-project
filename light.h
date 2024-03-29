/*
   File: light.h
   Description: Light objects.
   Author: Richard Váňa
   Date: May 23, 2023
*/

#pragma once

#include "generalObject.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <vector>

#define MAX_LIGHTS 16

// 0 - normal, 1 - reflektor, 2 - parallel

struct Light {
    glm::vec3 pos;
    glm::vec3 color;
    int type;
    glm::vec3 dir;
    float angle;
};

namespace lightManager {

    void init(shaderProgram** shaderArray, int shaderCount);
    
    void add(glm::vec3 pos, glm::vec3 clr);
    void addDir(glm::vec3 dir, glm::vec3 clr);
    void add(glm::vec3 pos, glm::vec3 clr, glm::vec3 dir, float angle);

    // sets the uniforms in the shaders
    void updateShaders();


    void setPos(int idx, glm::vec3 pos);
    void setDir(int idx, glm::vec3 dir);
};