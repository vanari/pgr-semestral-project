/*
   File: generalObject.cpp
   Description: Base for other objects in 3D space.
   Author: Richard Váňa
   Date: May 23, 2023
*/

#include "generalObject.h"

void generalObject::setPos(glm::vec3 pos) {
    position = pos;
}

void generalObject::setDir(glm::vec3 dir) {
    direction = dir;
}

void generalObject::translate(glm::vec3 translationVector) {
    position += translationVector;
}

glm::vec3 generalObject::getPos(){
    return position;
}

void generalObject::log() {
    std::cout << "Object coords: " << x << ' ' << y << ' ' << z << std::endl;
}