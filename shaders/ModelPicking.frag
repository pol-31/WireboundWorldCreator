#version 460 core
in vec3 normal;
in vec3 position;
in vec2 texcoord;

layout(location = 1) uniform uint id;

out uint out_id;

void main() {
	out_id = id;
}