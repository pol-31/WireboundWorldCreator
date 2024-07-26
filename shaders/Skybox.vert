#version 460 core
layout (location = 0) in vec3 in_vertex;

out vec3 frag_texcoord;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

void main() {
    frag_texcoord = in_vertex;

    mat4 mat_view_upd = mat4(mat3(camera.view));

    vec4 pos = camera.proj * mat_view_upd * vec4(in_vertex, 1.0);
    gl_Position = pos.xyww;
}  

