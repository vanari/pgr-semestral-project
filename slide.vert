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

uniform float explode;

uniform float time;

void main(){

    vec4 fragPosV4 = model * vec4(aPos, 1.0);

    fragPos = fragPosV4.xyz;

    fragNorm = mat3(transpose(inverse(model))) * aNorm.xyz;

    texCoord = aTexCoord + vec2(time, 0);
    
    vec3 myPos = aPos + explode * aNorm;

    gl_Position = projection * view * model * vec4(myPos, 1.0);
}