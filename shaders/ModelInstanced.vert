#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

layout(location = 3) in mat4 instance_model;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

layout(location = 0) uniform mat4 map_model;

out vec3 v_world_pos;
out vec3 v_normal;
out vec2 v_texcoord;

void main() {
	mat4 model = map_model * instance_model;
	mat4 mv = camera.view * model;

	vec4 world_pos = model * vec4(in_vertex, 1.0);
	gl_Position = camera.proj * camera.view * world_pos;

	v_world_pos = world_pos.xyz;

	mat3 normal_mat = transpose(inverse(mat3(model)));
	v_normal = normalize(normal_mat * in_normal);

	v_texcoord = in_texcoord;
}
