#version 460 core

layout (location = 0) in vec2 in_patch_vertex; // 0..1 corner

out VS_OUT {
    vec2 tc;
} vs_out;

void main() {
    int vertex_id = int(gl_InstanceID);
    int x = int(vertex_id & 63);
    int y = int(vertex_id >> 6);
    vec2 patch_offset = vec2(x, y) + in_patch_vertex;
    vs_out.tc = patch_offset / 64.0f;
    vec2 world = patch_offset - 32.0f;
    gl_Position = vec4(world.x, 0.0, world.y, 1.0);
//    int vertex_id = int(gl_InstanceID);
//    int x = int(vertex_id & 1023);
//    int y = int(vertex_id >> 10);
//    vec2 patch_offset = vec2(x, y) + in_patch_vertex;
//    vs_out.tc = patch_offset / 1024.0f;
//    vec2 world = patch_offset / 16.0f - 32.0f;
//    gl_Position = vec4(world.x, 0.0, world.y, 1.0);
}
