#version 460 core

out vec4 out_color;

layout(location = 1) uniform sampler2D tex_color;  // Diffuse color texture
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

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

layout(location = 8) uniform sampler2DArray  material_albedo;
layout(location = 9) uniform sampler2DArray  material_normal;
layout(location = 10) uniform sampler2DArray  material_metallic;
layout(location = 11) uniform sampler2DArray  material_roughness;
layout(location = 12) uniform sampler2DArray  material_ao;

layout(location = 13) uniform sampler2D tex_splatmap;

layout(location = 7) uniform mat4 transform;

in TES_OUT {
    vec2 tc;  // Texture coordinates
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

TerrainSample SampleTerrain(int layer) {
    float map_scale = transform[0][0];
    vec2 world_pos = fs_in.tc * 1024.0f;

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

    vec3 uv0 = vec3(WorldUV(world_pos, r0), layer);
    vec3 uv1 = vec3(WorldUV(world_pos, r1), layer);

    vec3 albedo0 = texture(material_albedo, uv0).rgb;
    vec3 albedo1 = texture(material_albedo, uv1).rgb;

    vec2 normal0 = texture(material_normal, uv0).rg;
    vec2 normal1 = texture(material_normal, uv1).rg;

    float ao0 = texture(material_ao, uv0).r;
    float ao1 = texture(material_ao, uv1).r;

    t.albedo = mix(albedo0, albedo1, w);
    t.normal = mix(normal0, normal1, w);
    t.ao     = mix(ao0, ao1, w);

    return t;
}

void main() {
    vec4 weights = texture(tex_splatmap, fs_in.tc);
    weights /= max(dot(weights, vec4(1.0)), 1e-5);
    vec3 albedo = vec3(0.0);
    vec3 normal = vec3(0.0);
    float ao = 0.0;
    for (int i = 0; i < 4; ++i) {
        float w = weights[i] * 4.0f;
        if (w < 0.001) continue;
        TerrainSample terrain = SampleTerrain(i);
        albedo    += w * terrain.albedo;
        normal    += w * UnpackNormal(terrain.normal);
        ao    += w * terrain.ao;
    }
    normal = normalize(normal);

    vec3 n_terrain = UnpackNormal(texture(tex_nmap, fs_in.tc).rg);

    normal = normalize(vec3(
                            n_terrain.xy + normal.xy,
                            n_terrain.z * normal.z));

    vec3 sun_direction = vec3(0.0f, 1.0f, -1.0f);
    float NdotL = max(dot(normal, normalize(-sun_direction)), 0.0);

    vec3 direct  = environment.sun_color * NdotL;
    vec3 ambient = vec3(0.1f, 0.1f, 0.1f) * ao;
    vec3 lighting = direct + ambient;
    out_color = vec4(albedo * lighting, 1.0);
}
