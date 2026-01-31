#version 460 core

out vec4 out_color;

// 0 = near, 1 = mid, 2 = far
layout(location = 0) uniform sampler2DArray tex_displacement;
layout(location = 1) uniform sampler2DArray tex_derivatives;
layout(location = 2) uniform sampler2DArray tex_turbulence;

layout(location = 3) uniform vec3 scales;

layout(std140, binding = 2) uniform Environment {
    vec3 sun_color;
    float _pad1;
    vec3 sun_direction;
    float wind_speed;
    vec2 wind_velocity;
    float time;
    float delta_time;
} environment;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

layout (location = 4) uniform sampler2D tex_foam;
layout (location = 15) uniform sampler2D tex_terrain_height;
layout(location = 16) uniform mat4 transform;
layout(location = 17) uniform vec4 water_albedo;

in TES_OUT {
    vec3 tc;
} fs_in;

struct WaveSample {
    vec4 derivatives;
    float turbulence;
};

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

WaveSample SampleWaves(int layer, float factor) {
    float map_scale = transform[0][0];
    vec2 world_pos = fs_in.tc.xz * 1024.0f;

    WaveSample wave;
    wave.turbulence = 0.0f;
    wave.derivatives = vec4(0.0f);

    float lod = MapScaleToLod(map_scale);

    int i0 = int(floor(lod));
    int i1 = min(i0 + 1, kRepeatCount - 1);
    float w = fract(lod);

    float r0 = kRepeats[i0] / factor;
    float r1 = kRepeats[i1] / factor;

    vec3 uv0 = vec3(WorldUV(world_pos, r0), layer);
    vec3 uv1 = vec3(WorldUV(world_pos, r1), layer);

    vec4 der0 = texture(tex_derivatives, uv0).rgba;
    vec4 der1 = texture(tex_derivatives, uv1).rgba;

    float turb0 = texture(tex_turbulence, uv0).r;
    float turb1 = texture(tex_turbulence, uv1).r;

    wave.derivatives = mix(der0, der1, w);
    wave.turbulence = mix(turb0, turb1, w);

    return wave;
}

void main() {
    vec2 tc = fs_in.tc.xz;
    WaveSample wave_sample = SampleWaves(0, 16.0f);
    WaveSample wave_sample1 = SampleWaves(1, 8.0f);
    WaveSample wave_sample2 = SampleWaves(2, 1.0f);
    wave_sample.derivatives += wave_sample1.derivatives;
    wave_sample.derivatives += wave_sample2.derivatives;
    wave_sample.turbulence += wave_sample1.turbulence;
    wave_sample.turbulence += wave_sample2.turbulence;
    //    wave_sample.derivatives /= 6.0f;
    wave_sample.turbulence /= 6.0f;

    vec2 slope = vec2(wave_sample.derivatives.x / (1 + wave_sample.derivatives.z),
    wave_sample.derivatives.y / (1 + wave_sample.derivatives.w));
    float norml_cadrof = 10.0f;
    vec3 normal = normalize(vec3(norml_cadrof * -slope.x, norml_cadrof * -slope.y, 1.0f));


    float map_scale = transform[0][0];
    float foamLOD = MapScaleToLod(map_scale) / float(kRepeatCount - 1);
    float jacobian = clamp(-wave_sample.turbulence, 0.0, 1.0) * foamLOD;

    float diffuse_factor = max(dot(normal.rbg, normalize(-environment.sun_direction)), 0.0);
    vec3 lighting = environment.sun_color * diffuse_factor;

    float terrain_height = texture(tex_terrain_height, tc).r;
    float height_difference = fs_in.tc.y - terrain_height;
    float shoreWidth = 0.5f;
    float alpha = smoothstep(0.0, shoreWidth, height_difference * 4.0f);

    float foamJacobian = clamp(-wave_sample.turbulence, 0.0, 1.0);
    float foamNoise = smoothstep(0.4, 0.8, wave_sample.turbulence);

    float foam_dyn   = mix(foamNoise, foamJacobian, foamLOD);
    float foam_shore = clamp((1.0 - alpha) * 4.0f, 0.0, 1.0);
    float foam = max(foam_dyn, foam_shore);
    //    foam = 0.0f;

    vec3 water_albedo = water_albedo.rgb; //todo;
    water_albedo = vec3(0.0f, 0.0f, 0.8f);
    vec3 water = water_albedo * lighting;
    vec3 foamColor = vec3(1.0f);
    float foamAlpha = texture(tex_foam, tc * 1024).r;
    vec3 finalColor = mix(water, foamColor, foam);
    float finalAlpha = mix(1.0f, foamAlpha * foam, foam);

    alpha = smoothstep(0.0, finalAlpha, alpha);

    out_color = vec4(finalColor, alpha);
}
