#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_tangent;

out vec4 vert_color;
out vec2 tex_coord;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform vec4 color;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

void main() {
    uint index = gl_InstanceID + gl_BaseInstance;
    vert_color = color;
    vec3 position = model[3].xyz;
    mat3 billboard = mat3(
        -camera.forward,
        camera.up,
        -camera.right
    );
    vec3 worldPos = billboard * in_vertex + position;
    gl_Position = camera.proj * camera.view * vec4(worldPos, 1.0);
    tex_coord = in_texcoord;
}