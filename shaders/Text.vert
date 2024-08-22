#version 460
layout (location = 0) in vec2 in_texcoord;

uniform vec2 scale;
uniform vec2 translate;

out vec2 texcoord;

void main() {
    // triangle strip
    vec2 positions[4] = vec2[4](
    vec2(1.0f, -1.0f),
    vec2(1.0f, 1.0f),
    vec2(-1.0f, -1.0f),
    vec2(-1.0f, 1.0f)
    );

    uint vert_idx = gl_VertexID & 3;

    gl_Position = vec4(
    positions[vert_idx].x * scale.x + translate.x,
    positions[vert_idx].y * scale.y + translate.y,
    -1.0f, 1.0f
    );
    texcoord = in_texcoord;
}
