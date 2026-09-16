#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 vert_color;
    mat3 TBN;
    flat uint material_id;
    flat uint use_triplanar;
} fs_in;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

layout(binding = 0) uniform sampler2DArray uAlbedo;
layout(binding = 1) uniform sampler2DArray uNormal;
layout(binding = 2) uniform sampler2DArray uRoughMetalAo;

layout(binding = 3) uniform sampler2DArray uAlbedoEmbroidery1;
layout(binding = 4) uniform sampler2DArray uAlbedoEmbroidery2;
layout(binding = 5) uniform sampler2DArray uAlbedoEmbroidery3;
layout(binding = 6) uniform sampler2DArray uAlbedoEmbroidery4;

void main() {
    vec3 baseAlbedo = vec3(1.0f);
    vec3 normalMapSample = vec3(0.5f, 0.5f, 1.0f);
    vec3 rough_metal_ao = vec3(0.0f, 0.0f, 0.0f);

    if (fs_in.use_triplanar != 0) {
        // --- TRIPLANAR LOGIC ---
        vec3 blendWeights = abs(fs_in.TBN[2]);
        blendWeights = (blendWeights - 0.2) * 7.0;
        blendWeights = max(blendWeights, 0.0);
        blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z);

        float worldScale = 1.0f / fs_in.use_triplanar;
        vec3 scaledPos = fs_in.FragPos * worldScale;
        float layer = float(fs_in.material_id);

        // Build 3D coordinates (UV + Layer)
        vec3 uvX = vec3(scaledPos.yz, layer);
        vec3 uvY = vec3(scaledPos.xz, layer);
        vec3 uvZ = vec3(scaledPos.xy, layer);

        // Albedo Triplanar
        vec4 colX = texture(uAlbedo, uvX);
        vec4 colY = texture(uAlbedo, uvY);
        vec4 colZ = texture(uAlbedo, uvZ);
        baseAlbedo = (colX * blendWeights.x + colY * blendWeights.y + colZ * blendWeights.z).rgb;

        // Normal Triplanar
        vec4 normX = texture(uNormal, uvX);
        vec4 normY = texture(uNormal, uvY);
        vec4 normZ = texture(uNormal, uvZ);
        normalMapSample = (normX * blendWeights.x + normY * blendWeights.y + normZ * blendWeights.z).rgb;
        if (fs_in.material_id == 2) {
            baseAlbedo = vec3(0.8f);
        }
    } else {
        // --- STANDARD UV LOGIC ---
        vec3 tc = vec3(fs_in.TexCoords, float(fs_in.material_id));

        if (fs_in.material_id < 5) {
            normalMapSample = texture(uNormal, tc).rgb;
            rough_metal_ao = texture(uRoughMetalAo, tc).rgb;
            baseAlbedo = texture(uAlbedo, tc).rgb;
        } else if (fs_in.material_id == 7) {
            baseAlbedo = texture(uAlbedoEmbroidery1, tc).rgb;
        } else if (fs_in.material_id == 8) {
            baseAlbedo = texture(uAlbedoEmbroidery2, tc).rgb;
        } else if (fs_in.material_id == 9) {
            baseAlbedo = texture(uAlbedoEmbroidery3, tc).rgb;
        } else if (fs_in.material_id == 10) {
            baseAlbedo = texture(uAlbedoEmbroidery4, tc).rgb;
        } else {
            baseAlbedo = vec3(1.0f);
        }
    }

    // Apply vertex color tinting once at the end
    gAlbedoSpec.rgb = baseAlbedo * fs_in.vert_color.rgb;

    vec3 tangentNormal = normalMapSample * 2.0 - 1.0;
    mat3 TBN = mat3(normalize(fs_in.TBN[0]), normalize(fs_in.TBN[1]), normalize(fs_in.TBN[2]));

    gNormal = vec4(normalize(TBN * tangentNormal), 1.0f);
    gPosition = vec4(fs_in.FragPos, 1.0f);
    gAlbedoSpec.a = 1.0f;
}