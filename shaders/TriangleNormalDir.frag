#version 460 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    vec4 vert_color;
} fs_in;

out vec4 FragColor;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

layout (location = 1) uniform sampler2D shadowMap;
layout (location = 2) uniform vec3 lightPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }
    return shadow;
}

void main() {
    vec3 color = vec3(1.0f);
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);

    // Фоновая составляющая
    vec3 ambient = 0.15 * color;

    // Диффузная составляющая
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // Отраженная составляющая
    vec3 viewDir = normalize(camera.pos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // Вычисляем тень
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow *  1.4f) * (diffuse + specular)) * color;

//    FragColor = vec4(0.0);
    FragColor = vec4(lighting * 0.5f, 1.0);
}
