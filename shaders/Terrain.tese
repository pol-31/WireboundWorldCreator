#version 460 core
layout(quads, fractional_even_spacing, ccw) in;

in TCS_OUT {
    vec2 tc_patchUV;
    flat int patchID;
} tes_in[];

out TES_OUT {
    vec3 tc;
} tes_out;

struct PatchGPU {
    vec4 p0;
    vec4 p1;
    vec4 heightLod_pad;
};

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

layout(std140, binding = 5) readonly buffer Patches {
    PatchGPU patch_gpu[];
} patches;

layout (location = 0) uniform sampler2D tex_displacement;

void main() {
    PatchGPU p = patches.patch_gpu[tes_in[0].patchID];
    vec2 uv = gl_TessCoord.xy;
    vec2 worldXZ =
    mix(
        mix(p.p0.xy, p.p1.xy, uv.x),
        mix(p.p0.zw, p.p1.zw, uv.x),
        uv.y
    );
    vec2 heightUV = (worldXZ + 32.0) / 64.0;
    float h = texture(tex_displacement, heightUV).r;
    tes_out.tc = vec3(worldXZ.x, h, worldXZ.y);
    gl_Position = camera.proj * camera.view * /**transform * */vec4(tes_out.tc, 1.0);
}
//void main() {
//    PatchGPU p = patches.patch_gpu[tes_in[0].patchID];
//    vec2 uv = gl_TessCoord.xy;
//    vec2 worldXZ =
//    mix(
//        mix(p.p0.xy, p.p1.xy, uv.x),
//        mix(p.p0.zw, p.p1.zw, uv.x),
//        uv.y
//    );
//    //    worldXZ = vec2(ivec2(worldXZ * 16.0f + 512.0f)) / 16.0f - 32.0f;
//    vec2 heightUV = (worldXZ + 32.0) / 64.0;
//    //    float h = textureLod(tex_displacement, hUV, p.heightLod).r;
//    float h = texture(tex_displacement, heightUV).r;
//    //    h = 0.0f;
//    tes_out.tc = vec3(worldXZ.x, h, worldXZ.y);
//    gl_Position = camera.proj * camera.view * /**transform * */vec4(tes_out.tc, 1.0);
//    //tes_out.tc = vec3(heightUV.x, h, heightUV.y);
//}