#version 460
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

layout(location = 0) uniform vec2 scale;
layout(location = 1) uniform vec2 translate;

out vec2 texcoords;

void main() {
    gl_Position = vec4(
      in_position.x * scale.x + translate.x,
      in_position.y * scale.y + translate.y,
      -1.0f, 1.0f
    );
    texcoords = in_texcoord;
}