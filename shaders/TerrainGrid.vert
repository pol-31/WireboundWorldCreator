#version 460 core

layout(location = 0) in vec3 in_position;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

out vec3 world_pos;

void main() {
    world_pos = in_position;
    gl_Position = camera.proj * camera.view * vec4(in_position, 1.0);
}