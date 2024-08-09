#version 460 core


layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

// we don't rotate and draw in 2d, so:
// first row for transform, second for scale
//uniform mat2 transform;

out vec2 texcoord;

void main() {
    // TODO: simply mat multiplication
    vec2 pos = in_position/* * vec2(transform[1]) + vec2(transform[0])*/;
    gl_Position = vec4(pos, 0.0f, 1.0);
    texcoord = in_texcoord;
}
