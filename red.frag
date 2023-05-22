#version 330 core

out vec4 fragColor;

uniform sampler2D objTexture;

in vec2 texCoord;

void main() {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}