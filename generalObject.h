/*
   File: generalObject.h
   Description: Base for other objects in 3D space.
   Author: Richard Váňa
   Date: May 23, 2023
*/

#pragma once

#include <glm/glm.hpp>
#include <iostream>

class generalObject {
    protected:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
    float &x = position.x;
    float &y = position.y;
    float &z = position.z;


    public:
    void setPos(glm::vec3 pos);
    void setDir(glm::vec3 dir);
    void translate(glm::vec3 translationVector);

    // outputs objects position
    void log();

    glm::vec3 getPos();
};