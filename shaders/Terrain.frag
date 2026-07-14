#version 460 core

out vec4 out_color;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

layout(location = 1) uniform sampler2DArray  material_albedo;
layout(location = 2) uniform sampler2DArray  material_normal;
layout(location = 3) uniform sampler2DArray  material_rough_metal_ao;
layout(location = 4) uniform int material_id;

in TES_OUT {
    vec3 tc;  // Texture coordinates
} fs_in;

vec3 UnpackNormal(vec2 nmap_rg) {
    return normalize(vec3(nmap_rg * 2.0 - 1.0, sqrt(1.0 - dot(nmap_rg * 2.0 - 1.0, nmap_rg * 2.0 - 1.0))));
}

vec2 WorldUV(vec2 world_xz, float repeats) {
    // 1024 texture, 64m tile → adjust if needed
    return world_xz / 64.0 * repeats;
}
vec2 WorldUVY(vec2 world_xz, float repeats) {
    // 1024 texture, 64m tile → adjust if needed
    return vec2(world_xz.x / 64.0 * repeats, world_xz.y);
}

void main() {
    vec3 worldPos = fs_in.tc * 128.0f;
    worldPos.y /= 100.0f;
    vec2 uvY = worldPos.xz; // Y axis → XZ (this is your height-safe one)
    vec3 albedo = texture(material_albedo, vec3(WorldUV(uvY, 1.0f), material_id)).rgb;
    vec3 n = texture(material_normal, vec3(WorldUV(uvY, 1.0f), material_id)).rgb;
    vec3 rma = texture(material_rough_metal_ao, vec3(WorldUV(uvY, 1.0f), material_id)).rgb;
    out_color = vec4(albedo, 1.0);
//    out_color = vec4(1.0f);


    //    vec3 sun_direction = vec3(0.0f, 0.1f, -0.2f);
    //    vec3 n_terrain = UnpackNormal(texture(tex_nmap, fs_in.tc.xz).rg);
    //    float NdotL = max(dot(n_terrain.rgb, normalize(-sun_direction)), 0.0);
    //    vec3 direct  = vec3(1.0f) * NdotL;
    //    vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
    //    vec3 lighting = direct + ambient;
}