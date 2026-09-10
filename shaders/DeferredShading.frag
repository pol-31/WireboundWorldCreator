#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout (location = 0) uniform sampler2D gPosition;
layout (location = 1) uniform sampler2D gNormal;
layout (location = 2) uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;
};

const float LightLinear = 0.09;
const float LightQuadratic = 0.032;
//const float LightLinear = 0.9;
//const float LightQuadratic = 4.5;

const int MAX_LIGHTS = 8;
layout (location = 3) uniform int lights_num;
layout (location = 4) uniform Light lights[MAX_LIGHTS];

uniform samplerCubeArray shadowMaps[MAX_LIGHTS];
uniform float farPlanes[MAX_LIGHTS];

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

float ShadowCalculation(int lightIndex, vec3 fragPos, vec3 lightPos) {
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(shadowMaps[lightIndex], vec4(fragToLight, 0)).r;
    closestDepth *= farPlanes[lightIndex];
    float currentDepth = length(fragToLight);

    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main() {

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 ambient = Diffuse * 0.05;
    vec3 lighting = ambient;

    vec3 viewDir  = normalize(camera.pos - FragPos);
    for(int i = 0; i < lights_num; ++i) {
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;

        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 64.0);
        vec3 specular = lights[i].Color * spec * Specular;

        float dist = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + LightLinear * dist + LightQuadratic * dist * dist);

        diffuse *= attenuation;
        specular *= attenuation;

        float shadow = ShadowCalculation(i, FragPos, lights[i].Position);
        lighting += (1.0 - shadow) * (diffuse + specular);
    }
    FragColor = vec4(lighting, 1.0);
}