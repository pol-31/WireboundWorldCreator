#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

out vec3 vert_color;

struct InstanceData {
    mat4 modelMatrix;
    vec4 modelColor;
};

layout(std430, binding = 0) buffer InstanceBuffer {
    InstanceData instances[];
};

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
    mat4 model = instances[index].modelMatrix;
    vert_color = vec3(instances[index].modelColor);
    vec4 worldPos = model * vec4(in_vertex, 1.0);
    gl_Position = camera.proj * camera.view * worldPos;
}