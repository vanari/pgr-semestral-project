#version 330 core

out vec4 fragColor;


in vec2 texCoord;

uniform float time;

uniform float baseFogDensity;

uniform int osc;

uniform sampler2D objTexture;
uniform sampler2D objTexture1;

in vec3 fragPos;
in vec3 fragNorm;

uniform vec3 camPos;



void main() {

    float dist = distance(fragPos, camPos);

    float fogDensity = baseFogDensity * 0.5 * (1.0 + sin(5*time));
    
    float fogFactor = 1.0 - exp(-fogDensity * dist);

    vec4 fogColor = mix(vec4(1.0, 0.0, 1.0, 1.0), vec4(1.0, 1.0, 0, 1.0), .5*(1.0 + sin(2*time)));

    vec4 litColor = vec4(texture(objTexture, texCoord).xyz, 0.1);

    fragColor = mix(litColor, fogColor, fogFactor);
}