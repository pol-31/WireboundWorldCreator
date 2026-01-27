#version 460 core

out vec4 FragColor;

layout (location = 2) uniform vec4 color;

void main() {
    FragColor = vec4(color.rgb, 1.0f);
}