#version 460 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

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

//vec3 UnpackNormal(vec2 nmap_rg) {
//    return normalize(vec3(nmap_rg * 2.0 - 1.0, sqrt(1.0 - dot(nmap_rg * 2.0 - 1.0, nmap_rg * 2.0 - 1.0))));
//}

vec2 WorldUV(vec2 world_xz, float repeats) {
    // 1024 texture, 64m tile → adjust if needed
    return world_xz / 64.0 * repeats;
}
vec2 WorldUVY(vec2 world_xz, float repeats) {
    // 1024 texture, 64m tile → adjust if needed
    return vec2(world_xz.x / 64.0 * repeats, world_xz.y);
}

vec3 UnpackNormal(vec2 nmap_rg) {
    vec2 xy = nmap_rg * 2.0 - 1.0;
    float z = sqrt(max(1.0 - dot(xy, xy), 0.0));
    return normalize(vec3(xy, z));
}

void main() {
    // 1. CLEANER UVs:
    // fs_in.tc is already in world space. You don't need convoluted math to get UVs.
    // Just divide world XZ by your desired tile scale (e.g., 64.0 meters)
    vec2 uv = fs_in.tc.xz / 64.0;

    // 2. Sample the textures
    vec3 albedo = texture(material_albedo, vec3(uv, material_id)).rgb;
    vec3 rma = texture(material_rough_metal_ao, vec3(uv, material_id)).rgb;

    // Unpack Tangent Space normal (where Z is "UP")
    vec2 raw_normal = texture(material_normal, vec3(uv, material_id)).rg;
    vec3 ts_normal = UnpackNormal(raw_normal);

    // 3. GET TRUE TERRAIN SLOPE NORMAL (Macro Normal)
    // Because your TES displaces vertices, we use screen-space derivatives to
    // instantly calculate the exact world-space geometric normal of the hills!
    vec3 dpdx = dFdx(fs_in.tc);
    vec3 dpdy = dFdy(fs_in.tc);
    vec3 geo_normal = normalize(cross(dpdx, dpdy));

    // Ensure the normal always points UP (fixes flipping when looking backward)
    if (geo_normal.y < 0.0) {
        geo_normal = -geo_normal;
    }

    // 4. CONSTRUCT TBN MATRIX
    // Maps the Tangent Space normal (Z-up) to World Space (Y-up, conforming to hills)
    vec3 T = normalize(vec3(1.0, 0.0, 0.0));
    T = normalize(T - dot(T, geo_normal) * geo_normal); // Orthogonalize
    vec3 B = cross(geo_normal, T);
    mat3 TBN = mat3(T, B, geo_normal);

    // 5. Final World Space Normal
    vec3 final_normal = normalize(TBN * ts_normal);

    // 6. OUTPUT TO G-BUFFER
    gPosition = vec4(fs_in.tc, 1.0);
    gNormal = vec4(final_normal, 1.0);     // Now strictly in World Space [-1, 1] !

    // Assuming you pack Roughness/Specular into the alpha channel for the deferred pass
    gAlbedoSpec = vec4(albedo, rma.r);
}
//
//void main() {
//    vec3 heightUV = (fs_in.tc.xyz + 32.0) / 64.0;
//    vec3 worldPos = heightUV * 128.0f / 0.1f;
//    worldPos.y /= 100.0f;
//    vec2 uvY = worldPos.xz; // Y axis → XZ (this is your height-safe one)
//    vec3 albedo = texture(material_albedo, vec3(WorldUV(uvY, 1.0f), material_id)).rgb;
//    vec3 n = texture(material_normal, vec3(WorldUV(uvY, 1.0f), material_id)).rgb;
//    vec3 rma = texture(material_rough_metal_ao, vec3(WorldUV(uvY, 1.0f), material_id)).rgb;
//    gAlbedoSpec = vec4(albedo, 1.0);
//    gPosition = vec4(fs_in.tc, 1.0f);
//
//
//        vec3 sun_direction = vec3(0.0f, 0.1f, -0.2f);
//        vec3 n_terrain = UnpackNormal(texture(material_normal, vec3(WorldUV(uvY, 1.0f), material_id)).rg);
////        float NdotL = max(dot(n_terrain.rgb, normalize(-sun_direction)), 0.0);
////        vec3 direct  = vec3(1.0f) * NdotL;
////        vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
////        vec3 lighting = direct + ambient;
//
//    gNormal = vec4(n_terrain, 1.0f);
//    gNormal = vec4(0.5f, 1.0f, 0.5f, 1.0f);
//    gNormal = vec4(n, 1.0f);
//    //gAlbedoSpec = gNormal;
//}