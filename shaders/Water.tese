#version 460 core

//layout (quads, fractional_odd_spacing) in;
layout (quads, equal_spacing, ccw) in;

layout(location = 0) uniform sampler2DArray tex_displacement;
layout(location = 3) uniform vec3 scales;
layout(location = 16) uniform mat4 transform;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

in TCS_OUT {
    vec2 tc;
} tes_in[];

out TES_OUT {
    vec2 tc;
    float height;
    float map_scale;
} tes_out;

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

void main(void) {
    float map_scale = transform[0][0];
    tes_out.map_scale = map_scale;
    vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);
    vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);
    vec2 tc = mix(tc2, tc1, gl_TessCoord.y);
    vec4 p1 = mix(gl_in[0].gl_Position,  gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    vec4 p = mix(p2, p1, gl_TessCoord.y);
    for (int i = 0; i < 3; ++i) {
        p.xyz += SampleWaves(i, 16.0f, tc);
        p.xyz += SampleWaves(i, 8.0f, tc);
        p.xyz += SampleWaves(i, 1.0f, tc);
    }
    tes_out.height = p.y;
    p = transform * p;
    gl_Position = camera.proj * camera.view * p;
    tes_out.tc = tc;
}
