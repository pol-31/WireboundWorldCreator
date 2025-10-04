#version 460 core

layout(location = 0) in vec2 in_position;
layout(location = 1) in float in_t;

layout(location = 0) uniform vec2 pos_offset; // e.g. move to cursor pos

out float v_t;

void main() {
    gl_Position = vec4(in_position + pos_offset, 0.0f, 1.0);
    v_t = in_t;
}