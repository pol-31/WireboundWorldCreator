#version 460 core
layout (location = 0) out vec4 FragColor;

in vec4 vert_color;

void main() {
    FragColor = vec4(vert_color.xyz, 1.0);
    FragColor = vec4(1.0);
}