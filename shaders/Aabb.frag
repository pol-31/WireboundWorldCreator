#version 460 core

layout (location = 4) uniform vec4 color;

out vec4 out_color;

void main() {
    out_color = vec4(color);
}