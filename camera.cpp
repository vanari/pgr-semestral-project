#include "camera.h"

camera::camera(unsigned int numShaders, shaderProgram** sList) : nShaders(numShaders), shaderList(sList) {
    calculate();
}

void camera::updateShaders() {
    for (int i = 0; i < nShaders ; ++i) {
        shaderProgram *shader = shaderList[i];

        shader->use();

        shader->setUniform<glm::mat4>("projection", projectionMatrix);
        shader->setUniform<glm::mat4>("view", viewMatrix);

        glUseProgram(0);
    }
}

void camera::setDir(glm::vec3 dir) {
    direction = dir;
    calculate();
}

void camera::setUpDir(glm::vec3 upDir) {
    upDirection = upDir;
    calculate();
}

void camera::calculate() {
    glm::mat4 worldRotation = glm::mat4(1.0f);
    worldRotation = glm::rotate(worldRotation, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    viewMatrix = glm::lookAt(position, position + direction, upDirection);
    viewMatrix = viewMatrix * worldRotation;
    projectionMatrix = glm::perspective(fov, ratio, nearPlane, farPlane);
}

void camera::setRatio(float newRatio) {
    ratio = newRatio;
}