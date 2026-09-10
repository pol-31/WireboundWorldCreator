#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout(location = 0) uniform mat4 model;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

out vec3 WorldPos;
out vec3 Normal;
out vec3 ViewDir;
out vec3 FragPos;

void main() {
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    ViewDir = normalize(camera.pos - WorldPos);

    FragPos = WorldPos.xyz;

    gl_Position = camera.proj * camera.view * vec4(WorldPos, 1.0);
}