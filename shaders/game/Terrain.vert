#version 460 core
layout (location = 0) in vec2 in_patch_vertex; // 0..1 corner
out VS_OUT {
    vec2 v_patchUV;
    flat int patchID;
} vs_out;
void main() {
    vs_out.v_patchUV = in_patch_vertex;
    vs_out.patchID = int(gl_InstanceID);
}