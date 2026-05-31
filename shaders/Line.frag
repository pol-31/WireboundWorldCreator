#version 460 core

in float vert_color;

out vec4 out_color;

void main() {
    out_color = vec4(vert_color);
}