#version 460 core

layout(location = 0) in vec3 in_position;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

out vec3 world_pos;

void main() {
    world_pos = in_position;
    gl_Position = camera.proj * camera.view * vec4(in_position, 1.0);
}