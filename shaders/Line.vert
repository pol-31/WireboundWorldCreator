#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in float in_color;

out float vert_color;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

void main(){
    vec4 world_pos = vec4(in_vertex, 1.0);
	gl_Position = camera.proj * camera.view * world_pos;
    vert_color = in_color;
}
