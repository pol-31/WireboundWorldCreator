#version 460 core

layout (location = 0) in vec2 in_patch_vertex; // 0..1 corner
layout (location = 1) in uint vertex_id;

out VS_OUT {
    vec2 tc;
} vs_out;

void main() {
    int x = int(vertex_id & 63);
    int y = int(vertex_id >> 6);
    vec2 patch_offset = vec2(x, y);
    vec2 world = patch_offset + in_patch_vertex;
    vs_out.tc = world / 64.0f;
    gl_Position = vec4(world.x - 32.0, 0.0, world.y - 32.0, 1.0);
}
