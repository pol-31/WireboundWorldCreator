#version 460 core
layout (location = 0) in vec4 in_vertex; // xyz + id (we don't use id here)

uniform mat4 transform;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

void main() {
    gl_Position = camera.proj * camera.view * transform * in_vertex;
}  

