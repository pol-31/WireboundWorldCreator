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

const float LightLinear = 10.1;
const float LightQuadratic = 0.2;

const int MAX_LIGHTS = 32;
layout (location = 3) uniform int lights_num;
layout (location = 4) uniform Light lights[MAX_LIGHTS];


layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

void main() {
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 lighting  = Diffuse * 0.1;
    vec3 viewDir  = normalize(camera.pos - FragPos);
    for(int i = 0; i < lights_num; ++i) {
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;

        float dist = length(lights[i].Position - FragPos);
//        dist = 0.0f;
        float attenuation = 1.0 / (1.0 + LightLinear * dist
          + LightQuadratic * dist * dist);
//        attenuation = 1.0f / dist;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
    }
    FragColor = vec4(lighting, 1.0);
//    FragColor = vec4(0.5);
}