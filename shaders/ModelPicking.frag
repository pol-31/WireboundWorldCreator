#version 460 core

layout(location = 1) uniform uint id;

out uint out_id;

void main() {
	out_id = id;
}