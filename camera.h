#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class camera {
    private:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    
    float fov = glm::radians(45.0f);
    float ratio = 1.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix = glm::mat4(1.0f);

    shaderProgram **shaderList = nullptr;
    unsigned int nShaders = 0;

    void calculate();

    public:
    camera(unsigned int numShaders, shaderProgram** sList);
    void updateShaders();
    void setProjectionMatrix();
    void setViewMatrix();
    void setPos(glm::vec3 pos);
    void setDir(glm::vec3 dir);
    void setUpDir(glm::vec3 upDir);
};