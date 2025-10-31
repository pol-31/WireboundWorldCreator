#version 460 core

layout(location = 0) in vec2 in_position;

layout(location = 0) uniform vec2 pos_offset; // e.g. move to cursor pos

void main() {
    gl_Position = vec4(in_position + pos_offset, 0.0f, 1.0);
}