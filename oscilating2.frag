#version 330 core

out vec4 fragColor;

uniform float time;

uniform sampler2D objTexture;
uniform sampler2D objTexture1;

in vec2 texCoord;

void main() {
    vec4 myColor1 = texture(objTexture, texCoord); 
    vec4 myColor2 = texture(objTexture1, texCoord);
    fragColor = mix(myColor1, myColor2, .5 + .5*sin(20*time));//texture(objTexture, texCoord);//vec4(1.0, 1.0, 1.0, 1.0);
}