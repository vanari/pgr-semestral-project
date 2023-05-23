#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float screenRatio;

out vec2 texCoord;

void main(){
    float scale = 0.03;
    vec3 rescaledPos = vec3(scale*aPos.x, scale*screenRatio*aPos.y, -1.0);
    gl_Position = vec4(rescaledPos, 1.0);

    texCoord = aTexCoord;
}