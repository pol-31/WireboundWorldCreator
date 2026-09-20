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
layout(binding = 2) uniform sampler2DArray uAoRoughMetal;
// DEPRECATED: uRoughMetalAo;

layout(binding = 3) uniform sampler2DArray uAlbedoEmbroidery1;
layout(binding = 4) uniform sampler2DArray uAlbedoEmbroidery2;
layout(binding = 5) uniform sampler2DArray uAlbedoEmbroidery3;
layout(binding = 6) uniform sampler2DArray uAlbedoEmbroidery4;

// 1. Core Triplanar Sampler: Samples ONE scale of the texture
vec3 SampleTriplanar(sampler2DArray tex, vec3 scaledPos, vec3 weights, float layer) {
    vec3 uvX = vec3(scaledPos.yz, layer);
    vec3 uvY = vec3(scaledPos.xz, layer);
    vec3 uvZ = vec3(scaledPos.xy, layer);

    return texture(tex, uvX).rgb * weights.x +
    texture(tex, uvY).rgb * weights.y +
    texture(tex, uvZ).rgb * weights.z;
}

vec3 SampleTriplanar3Level(sampler2DArray tex, vec3 fragPos, vec3 weights, float layer,
                           float scaleClose, float scaleMid, float scaleFar,
                           float blendCloseMid, float blendMidFar) {

    vec3 valClose = SampleTriplanar(tex, fragPos * scaleClose, weights, layer);
    vec3 valMid   = SampleTriplanar(tex, fragPos * scaleMid,   weights, layer);
    vec3 valFar   = SampleTriplanar(tex, fragPos * scaleFar,   weights, layer);

    // Mix Close -> Mid first, then mix that result -> Far
    vec3 blendedResult = mix(valClose, valMid, blendCloseMid);
    return mix(blendedResult, valFar, blendMidFar);
}

vec3 SampleTriplanar2Level(sampler2DArray tex, vec3 fragPos, vec3 weights, float layer,
                           float scaleClose, float scaleFar, float blendCloseFar) {

    vec3 valClose = SampleTriplanar(tex, fragPos * scaleClose, weights, layer);
    vec3 valFar   = SampleTriplanar(tex, fragPos * scaleFar,   weights, layer);

    return mix(valClose, valFar, blendCloseFar);
}

void main() {
    vec3 baseAlbedo = vec3(1.0f);
    vec3 normalMapSample = vec3(0.5f, 0.5f, 1.0f);
    vec3 ao_rough_metal = vec3(0.0f, 0.6f, 1.0f);

    if (fs_in.use_triplanar != 0) {
        // --- TRIPLANAR WEIGHTS ---
        vec3 blendWeights = abs(fs_in.TBN[2]);
        blendWeights = (blendWeights - 0.2) * 7.0;
        blendWeights = max(blendWeights, 0.0);
        blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z);

        // --- DISTANCES FOR INTERIORS ---
        float distToCam = length(camera.pos - fs_in.FragPos);

        // Super Close (0m to 3m) blending into Mid (3m to 6m)
        float blendCloseMid = smoothstep(3.0, 6.0, distToCam);

        // Mid (6m to 15m) blending into Far (15m to 25m corridor length)
        float blendMidFar = smoothstep(15.0, 25.0, distToCam);

        float blendCloseFar = smoothstep(3.0, 10.0, distToCam);

        // --- SCALES ---
        float baseScale = (1.0f / fs_in.use_triplanar);
        float scaleClose = baseScale * 4.0f;  // Extreme macro detail (mud wall cracks)
        float scaleMid   = baseScale * 1.0f;  // Normal room scale
        float scaleFar   = baseScale * 0.5f;  // Low frequency to break tiling down halls

        float layer = float(fs_in.material_id);

        // --- THE MAGIC: 3 Lines of Code ---

        if (fs_in.material_id == 2) {
            // clay (walls, ceiling)
            baseAlbedo = vec3(0.9f);
            ao_rough_metal.r = 1.0f;
            ao_rough_metal.g = 0.85f;
            ao_rough_metal.b = 0.0f;
        } else {
            baseAlbedo      = SampleTriplanar2Level(uAlbedo,       fs_in.FragPos, blendWeights, layer, scaleClose, scaleFar, blendCloseFar);
            normalMapSample = SampleTriplanar2Level(uNormal,       fs_in.FragPos, blendWeights, layer, scaleClose, scaleFar, blendCloseFar);
            ao_rough_metal  = SampleTriplanar2Level(uAoRoughMetal, fs_in.FragPos, blendWeights, layer, scaleClose, scaleFar, blendCloseFar);
//            baseAlbedo      = SampleTriplanar3Level(uAlbedo,       fs_in.FragPos, blendWeights, layer, scaleClose, scaleMid, scaleFar, blendCloseMid, blendMidFar);
//            normalMapSample = SampleTriplanar3Level(uNormal,       fs_in.FragPos, blendWeights, layer, scaleClose, scaleMid, scaleFar, blendCloseMid, blendMidFar);
//            ao_rough_metal  = SampleTriplanar3Level(uAoRoughMetal, fs_in.FragPos, blendWeights, layer, scaleClose, scaleMid, scaleFar, blendCloseMid, blendMidFar);
        }
    } else {
        // --- STANDARD UV LOGIC ---
        vec3 tc = vec3(fs_in.TexCoords, float(fs_in.material_id));

        if (fs_in.material_id < 5) {
            normalMapSample = texture(uNormal, tc).rgb;
            ao_rough_metal = texture(uAoRoughMetal, tc).rgb;
            baseAlbedo = texture(uAlbedo, tc).rgb;
        } else {
            ao_rough_metal = vec3(1.0f, 0.9f, 0.0f);
            //ao_rough_metal = texture(uAoRoughMetal, vec3(fs_in.TexCoords, 2.0f)).rgb;

            if (fs_in.material_id == 7) {
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
    }

    //ao_rough_metal.r = 1.0f;
    //ao_rough_metal.g = 0.99f;
    //ao_rough_metal.b = 0.1f;


    gPosition = vec4(fs_in.FragPos, ao_rough_metal.g);
    vec3 tangentNormal = normalMapSample * 2.0 - 1.0;
    mat3 TBN = mat3(normalize(fs_in.TBN[0]), normalize(fs_in.TBN[1]), normalize(fs_in.TBN[2]));
    gNormal = vec4(normalize(TBN * tangentNormal), ao_rough_metal.b);
    gAlbedoSpec = vec4(baseAlbedo * fs_in.vert_color.rgb, ao_rough_metal.r);
}