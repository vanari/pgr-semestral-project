#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoord;

void main(){
    //gl_Position = vec4(aPos, 1.0);
    vec3 nPos = aPos - vec3(0.0f, 0.0f, 3.0f);
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    texCoord = aTexCoord;
}