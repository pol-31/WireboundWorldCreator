#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

layout (location = 0) uniform mat4 model_mat;

layout(binding = 0) uniform CameraBufferObject {
	mat4 view;
	mat4 proj;
	vec3 pos;
} camera;

out vec3 v_normal;
out vec3 v_world_pos;
out vec2 v_texcoord;

void main(){
	vec4 world_pos = model_mat * vec4(in_vertex, 1.0);
	gl_Position = camera.proj * camera.view * world_pos;
	v_world_pos = world_pos.xyz;
	mat3 normal_mat = transpose(inverse(mat3(model_mat)));
	v_normal = normalize(normal_mat * in_normal);
	v_texcoord = in_texcoord;
}