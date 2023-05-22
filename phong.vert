#version 330 core

const int MAX_LIGHTS = 16;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoord;

out vec3 fragPos;
out vec3 fragNorm;


void main(){

    vec4 fragPosV4 = model * vec4(aPos, 1.0);

    fragPos = fragPosV4.xyz;

    fragNorm = aNorm.xyz;

    texCoord = aTexCoord;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}