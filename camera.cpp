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

void camera::setPos(glm::vec3 pos) {
    position = pos;
    calculate();
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
    viewMatrix = glm::lookAt(position, position + direction, upDirection);
    projectionMatrix = glm::perspective(fov, ratio, nearPlane, farPlane);
}