#version 460 core

out vec4 out_color;

layout(location = 2) uniform sampler2D tex_nmap;   // Normal map (GL_RG)
layout(location = 3) uniform sampler2D tex_occlusion;   // Normal map (GL_RG)

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

layout(location = 8) uniform sampler2DArray  material_albedo;
layout(location = 9) uniform sampler2DArray  material_normal;
layout(location = 10) uniform sampler2DArray  material_metallic;
layout(location = 11) uniform sampler2DArray  material_roughness;
layout(location = 12) uniform sampler2DArray  material_ao;

layout(location = 13) uniform sampler2D tex_splatmap;

layout(location = 7) uniform mat4 transform;
layout (location = 0) uniform sampler2D tex_displacement;

in TES_OUT {
    vec3 tc;
} fs_in;

struct TerrainSample {
    vec3 albedo;
    vec2 normal;
    float ao;
};

const float kRepeats[8] = float[](
1.0,   // far
2.0,
4.0,
8.0,
16.0,
32.0,
64.0,   // near
128.0   // near
);
const int kRepeatCount = 8;


vec3 UnpackNormal(vec2 nmap_rg) {
    return normalize(vec3(nmap_rg * 2.0 - 1.0, sqrt(1.0 - dot(nmap_rg * 2.0 - 1.0, nmap_rg * 2.0 - 1.0))));
}


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
vec2 WorldUVY(vec2 world_xz, float repeats) {
    // 1024 texture, 64m tile → adjust if needed
    return vec2(world_xz.x / 64.0 * repeats, world_xz.y);
}

TerrainSample SampleTerrain(int layer) {
    float map_scale = transform[0][0];

    vec2 enc = texture(tex_nmap, fs_in.tc.xz).rg;
    vec2 n_xy = enc * 2.0 - 1.0;
    float n_z = sqrt(max(1.0 - dot(n_xy, n_xy), 0.0));
    vec3 n = normalize(vec3(n_xy, n_z));
    n = n.xzy;
    vec3 w1 = abs(n);
    float slope = 4.0f;
    w1 = pow(w1, vec3(slope));
    w1 /= (w1.x + w1.y + w1.z);

    vec3 worldPos = fs_in.tc * 128.0f;
    worldPos.y /= 100.0f;
    vec2 uvX = worldPos.zy; // X axis → YZ
    vec2 uvY = worldPos.xz; // Y axis → XZ (this is your height-safe one)
    vec2 uvZ = worldPos.xy; // Z axis → XY

    TerrainSample t;
    t.albedo = vec3(0.0);
    t.normal = vec2(0.0);
    t.ao     = 0.0;

    float lod = MapScaleToLod(map_scale);

    int i0 = int(floor(lod));
    int i1 = min(i0 + 1, kRepeatCount - 1);
    float w = fract(lod);

    float r0 = kRepeats[i0];
    float r1 = kRepeats[i1];

    vec3 uv0 = vec3(WorldUV(uvY, r0), layer);
    vec3 uv0_xy = vec3(WorldUV(uvZ, r0), layer);
    vec3 uv0_zy = vec3(WorldUV(uvX, r0), layer);
    vec3 uv1 = vec3(WorldUV(uvY, r1), layer);
    vec3 uv1_xy = vec3(WorldUV(uvZ, r1), layer);
    vec3 uv1_zy = vec3(WorldUV(uvX, r1), layer);


    vec3 albedo0 = texture(material_albedo, uv0).rgb;
    vec3 albedo0_xy = texture(material_albedo, uv0_xy).rgb;
    vec3 albedo0_zy = texture(material_albedo, uv0_zy).rgb;

    vec3 albedo1 = texture(material_albedo, uv1).rgb;
    vec3 albedo1_xy = texture(material_albedo, uv1_xy).rgb;
    vec3 albedo1_zy = texture(material_albedo, uv1_zy).rgb;

    // mixed
    vec3 albedo = mix(albedo0, albedo1, w);
    vec3 albedo_xy = mix(albedo0_xy, albedo1_xy, w);
    vec3 albedo_zy = mix(albedo0_zy, albedo1_zy, w);

    t.albedo =
    albedo_zy * w1.x +
    albedo * w1.y    +
    albedo_xy * w1.z;
    return t;
}

void main() {
//    out_color = vec4(1.0f);
//    return;
//    TerrainSample terrain = SampleTerrain(3);
    vec3 albedo = vec3(0.0f);
    vec4 weights = texture(tex_splatmap, fs_in.tc.xz);
    weights /= max(dot(weights, vec4(1.0)), 1e-5);
    for (int i = 0; i < 4; ++i) {
        float w = weights[i] * 4.0f;
        if (w < 0.001) continue;
        TerrainSample terrain = SampleTerrain(i);
        albedo    += w * terrain.albedo;
    }
    vec3 n_terrain = UnpackNormal(texture(tex_nmap, fs_in.tc.xz).rg);

    float ao = texture(tex_occlusion, fs_in.tc.xz).r;

    vec3 sun_direction = vec3(0.0f, 0.1f, -0.2f);
    float NdotL = max(dot(n_terrain.rgb, normalize(-sun_direction)), 0.0);


    vec3 direct  = environment.sun_color * NdotL;
    vec3 ambient = vec3(0.1f, 0.1f, 0.1f) * ao;
    vec3 lighting = direct + ambient;
    vec3 river_color = vec3(0.2f, 0.2f, 0.6f);
    out_color = vec4(albedo * lighting, 1.0);
}
