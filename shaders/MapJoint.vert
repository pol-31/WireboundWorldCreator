#version 460 core
layout (location = 0) in vec2 in_position;

layout (location = 0) uniform sampler2D tex_hmap;
layout (location = 1) uniform mat4 map_model;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

void main() {
    vec2 tc = vec2(in_position) / 64.0f + 0.5f;
    float height = texture(tex_hmap, tc).r;
    vec4 world_pos = vec4(in_position.x, height, in_position.y, 1.0f);
    gl_Position = camera.proj * camera.view * map_model * world_pos;
}
