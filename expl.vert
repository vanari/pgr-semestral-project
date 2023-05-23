#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float screenRatio;

uniform float explTime;

out vec2 texCoord;

void main(){
    float scale = 0.5;
    vec3 rescaledPos = vec3(scale*aPos.x, scale*screenRatio*aPos.y, -0.99);
    gl_Position = vec4(rescaledPos, 1.0);

    int phase = int(explTime/5.0 * (6.0*8.0));
    //phase = 6*8 -1 - phase;

    //phase = 5;

    int xPos = phase % 8;
    int yPos = 5 - phase / 6;

    texCoord = aTexCoord + vec2(float(xPos) * 0.125, float(-yPos) * 0.167);
}