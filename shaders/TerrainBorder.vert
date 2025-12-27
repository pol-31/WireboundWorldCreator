#version 460 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 aTex;

layout(location = 0) uniform mat4 model_mat;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

out vec2 TexCoord;

void main() {
    gl_Position = camera.proj * camera.view * model_mat * vec4(inPos, 1.0);
    TexCoord = aTex;
}
