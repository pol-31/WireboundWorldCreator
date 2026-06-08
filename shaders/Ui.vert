#version 460 core
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;
layout (location = 2) in vec4 in_color;

out vec2 texcoord;
out vec4 color;

layout (location = 1) uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * vec4(in_position, 0.0, 1.0);
    texcoord = in_texcoord;
    color = in_color;
}