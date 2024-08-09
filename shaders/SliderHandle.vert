#version 460
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

uniform float pos_y_down_offset_;

out vec2 texcoords;

void main() {
    gl_Position = vec4(in_position.x, in_position.y - pos_y_down_offset_, 0.0f, 1.0f);
    texcoords = in_texcoord;
}