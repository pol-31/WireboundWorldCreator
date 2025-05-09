#version 460 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;
layout(location = 5) uniform float res_factor;

out vec2 texcoord;

void main() {
    gl_Position = vec4(in_position * vec2(res_factor, 1.0f), -1.0f, 1.0);
    texcoord = in_texcoord;
}
