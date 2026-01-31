#version 460 core
layout (vertices = 4) out;
in VS_OUT {
    vec2 v_patchUV;
    flat int patchID;
} tcs_in[];
out TCS_OUT {
    vec2 tc_patchUV;
    flat int patchID;
} tcs_out[];
void main() {
    tcs_out[gl_InvocationID].tc_patchUV = tcs_in[gl_InvocationID].v_patchUV;
    tcs_out[gl_InvocationID].patchID   = tcs_in[gl_InvocationID].patchID;
    if (gl_InvocationID == 0) {
        float tess = 64.0f; // test
        gl_TessLevelOuter[0] = tess;
        gl_TessLevelOuter[1] = tess;
        gl_TessLevelOuter[2] = tess;
        gl_TessLevelOuter[3] = tess;
        gl_TessLevelInner[0] = tess;
        gl_TessLevelInner[1] = tess;
    }
}



layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;
