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

layout(location = 0) uniform sampler2DArray tex_displacement;
layout(location = 3) uniform vec3 scales;
layout(location = 16) uniform mat4 transform;

layout (location = 5) uniform sampler2D tex_water_height;
const float kRepeats[8] = float[](
1.0,   // far
2.0,
4.0,
8.0,
16.0,
32.0,
64.0,
128.0
);

const int kRepeatCount = 8;

float MapScaleToLod(float map_scale) {
    float t = clamp(
        (log(map_scale) - log(0.05)) / (log(100.0) - log(0.05)),
        0.0, 1.0
    );
    return t * float(kRepeatCount - 1);
}

vec2 WorldUV(vec2 world_xz, float repeats) {
    // 1024 texture, 64m tile → adjust if needed
    return world_xz / 64.0 * repeats;
}

vec3 SampleWaves(int layer, float factor, vec2 tc) {
    float map_scale = transform[0][0];
    vec2 world_pos = tc * 1024.0f;

    vec3 height;

    float lod = MapScaleToLod(map_scale);

    int i0 = int(floor(lod));
    int i1 = min(i0 + 1, kRepeatCount - 1);
    float w = fract(lod);

    float r0 = kRepeats[i0] / factor;
    float r1 = kRepeats[i1] / factor;

    vec3 uv0 = vec3(WorldUV(world_pos, r0), layer);
    vec3 uv1 = vec3(WorldUV(world_pos, r1), layer);

    vec3 albedo0 = texture(tex_displacement, uv0).xyz;
    vec3 albedo1 = texture(tex_displacement, uv1).xyz;

    height = mix(albedo0, albedo1, w);

    return height;
}

void main() {
    PatchGPU patch_ = patches.patch_gpu[tes_in[0].patchID];
    vec2 uv = gl_TessCoord.xy;
    vec2 p_xz =
    mix(
        mix(patch_.p0.xy, patch_.p1.xy, uv.x),
        mix(patch_.p0.zw, patch_.p1.zw, uv.x),
        uv.y
    );
    vec2 tc = (p_xz + 32.0) / 64.0;
    vec4 p = vec4(p_xz.x, 0.0f, p_xz.y, 1.0f);
    for (int i = 0; i < 3; ++i) {
        p.xyz += SampleWaves(i, 16.0f, tc);
        p.xyz += SampleWaves(i, 8.0f, tc);
        p.xyz += SampleWaves(i, 1.0f, tc);
    }
    p.y += texture(tex_water_height, tc).r;
    gl_Position = camera.proj * camera.view * transform * p;
    tes_out.tc = vec3(tc.x, p.y, tc.y);
}