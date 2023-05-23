#version 330 core

const int MAX_LIGHTS = 16;

out vec4 fragColor;

uniform float time;

uniform float baseFogDensity;

uniform int osc;

uniform sampler2D objTexture;
uniform sampler2D objTexture1;

in vec2 texCoord;

in vec3 fragPos;
in vec3 fragNorm;

uniform vec3 camPos;

uniform vec3 ambientM;
uniform vec3 diffuseM;
uniform vec3 specularM;
uniform float shininessM;

uniform vec3 ambient;
uniform int nLights;
//uniform Light lightArray[MAX_LIGHTS];


uniform vec3 light_pos[MAX_LIGHTS];
uniform vec3 light_color[MAX_LIGHTS];
uniform int light_type[MAX_LIGHTS];
uniform vec3 light_dir[MAX_LIGHTS];
uniform float light_angle[MAX_LIGHTS];
uniform float light_intensity[MAX_LIGHTS];

void main() {

    vec3 totalLight = vec3(0.0, 0.0, 0.0);
    vec3 viewDir = normalize(camPos - fragPos);

    float coef = 1.0;

    for (int i = 0; i < nLights; ++i) {
        //Light thisLight = lightArray[i];

        vec3 lightDir = vec3(0.0, 0.0, 0.0);

        if (light_type[i] == 0) { // bodove
            lightDir = fragPos - light_pos[i];
            lightDir = normalize(lightDir);
            coef = 1.0;
        } else if (light_type[i] == 1) { // kuzel
            vec3 dir = fragPos - light_pos[i];
            dir = normalize(dir);
            if (abs(dot(normalize(light_dir[i]), dir)) < abs(cos(radians(light_angle[i])))) {
                coef = 0.0;
            } else {
                coef = 1.0;
            }
            lightDir = dir;
        } else if (light_type[i] == 2) { // rovina
            lightDir = light_dir[i];
            lightDir = normalize(lightDir);
            coef = 1.0;
        }

        // diffuse

        float cosine = max(0.0, -dot(fragNorm, lightDir));
        vec3 diffuse = cosine * light_color[i] * diffuseM;

        // specular

        vec3 reflectDir = reflect(-lightDir, fragNorm);
        float spec = pow(max(-dot(viewDir, reflectDir), 0.0), shininessM);
        vec3 specular = spec * light_color[i] * specularM;

        // ambient
        
        totalLight += coef * light_intensity[i] * (ambient * ambientM + diffuse + specular);

    }
    
    vec4 textureColor;
    vec4 myColor1 = texture(objTexture, texCoord); 
    
    if (osc > 0) {
        vec4 myColor2 = texture(objTexture1, texCoord);
        textureColor = mix(myColor1, myColor2, .5 + .5*sin(20*time));
    } else {
        textureColor = myColor1;
    }

    // FOG

    float dist = distance(fragPos, camPos);

    float fogDensity = baseFogDensity * 0.5 * (1.0 + sin(5*time));
    
    float fogFactor = 1.0 - exp(-fogDensity * dist);

    vec4 fogColor = mix(vec4(1.0, 0.0, 1.0, 1.0), vec4(1.0, 1.0, 0, 1.0), .5*(1.0 + sin(2*time)));

    vec4 litColor = vec4(totalLight, 1.0) * textureColor;//vec4(1.0, 1.0, 1.0, 1.0);

    fragColor = mix(litColor, fogColor, fogFactor);
}