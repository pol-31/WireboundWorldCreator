#version 460 core
out vec4 FragColor;

layout(location = 1) uniform vec4 color;

void main() {
    FragColor = color;
}

