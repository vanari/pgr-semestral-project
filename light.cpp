/*
   File: light.cpp
   Description: Light objects.
   Author: Richard Váňa
   Date: May 23, 2023
*/

#include "light.h"


namespace lightManager{
    static shaderProgram **shaders;
    static int nShaders = 0;

    static Light lights[MAX_LIGHTS];
    glm::vec3 light_pos[MAX_LIGHTS];
    glm::vec3 light_color[MAX_LIGHTS];
    int light_type[MAX_LIGHTS];
    glm::vec3 light_dir[MAX_LIGHTS];
    float light_angle[MAX_LIGHTS];
    float light_intensity[MAX_LIGHTS];
    static int nLights = 0;

    static glm::vec3 ambient = glm::vec3(.3f, .3f, .3f);
};

void lightManager::init(shaderProgram** shaderArray, int shaderCount) {
    shaders = shaderArray;
    nShaders = shaderCount;
}

void lightManager::updateShaders() {

    for (int i = 0; i < nShaders; ++i) {

        shaders[i]->use();
        shaders[i]->setUniform<glm::vec3>("ambient", ambient);
        shaders[i]->setUniform<GLint>("nLights", nLights);
        shaders[i]->setUniform1iv("light_type", light_type, nLights);
        shaders[i]->setUniform1fv("light_intensity", light_intensity, nLights);
        shaders[i]->setUniform1fv("light_angle", light_angle, nLights);
        shaders[i]->setUniform3fv("light_pos", light_pos, nLights);
        shaders[i]->setUniform3fv("light_color", light_color, nLights);
        shaders[i]->setUniform3fv("light_dir", light_dir, nLights);
        shaders[i]->use(false);
    }
}

void lightManager::add(glm::vec3 pos, glm::vec3 clr) {
    light_pos[nLights] = pos;
    light_intensity[nLights] = 1.0f;
    light_color[nLights] = clr;
    light_type[nLights] = 0;

    nLights++;
}

void lightManager::addDir(glm::vec3 dir, glm::vec3 clr) {
    light_dir[nLights] = dir;
    light_intensity[nLights] = 1.0f;
    light_color[nLights] = clr;
    light_type[nLights] = 2;

    nLights++;
}

void lightManager::add(glm::vec3 pos, glm::vec3 clr, glm::vec3 dir, float angle) {
    light_pos[nLights] = pos;
    light_intensity[nLights] = 1.0f;
    light_color[nLights] = clr;
    light_dir[nLights] = dir;
    light_angle[nLights] = angle;
    light_type[nLights] = 1;

    nLights++;
}

void lightManager::setPos(int idx, glm::vec3 pos) {
    light_pos[idx] = pos;
}

void lightManager::setDir(int idx, glm::vec3 dir) {
    light_dir[idx] = dir;
}