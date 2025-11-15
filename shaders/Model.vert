#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

layout (location = 0) uniform mat4 model_mat;

layout(binding = 0) uniform CameraBufferObject {
	mat4 view;
	mat4 proj;
} camera;

out vec3 normal;
out vec3 position;
out vec2 texcoord;

void main(){
	mat4 mvp = camera.proj * camera.view * model_mat;
	gl_Position = mvp * vec4(in_vertex, 1);
	position = gl_Position.xyz;
	normal = normalize(mat3(mvp) * in_normal);
	position = in_vertex;
	texcoord = in_texcoord;
}