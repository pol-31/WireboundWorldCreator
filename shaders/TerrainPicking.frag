#version 460 core

out vec4 vert_data;
in vec4 in_vert_data;

void main(void) {
    vert_data = in_vert_data;
}