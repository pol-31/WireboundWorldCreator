#version 460 core
//layout (location = 0) out vec4 gPosition;
layout (location = 0) out vec4 gNormal;
layout (location = 1) out vec4 gAlbedoSpec;

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
layout(binding = 7) uniform sampler2DArray uAlbedoSkyrim;

layout(location = 8) uniform int uTriplanarSize;
layout(location = 9) uniform sampler2D uPerlin;

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


vec2 OctWrap(vec2 v) {
    return (1.0 - abs(v.yx)) * (vec2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0));
}

vec2 EncodeNormal(vec3 n) {
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    vec2 enc = n.z >= 0.0 ? n.xy : OctWrap(n.xy);
    return enc * 0.5 + 0.5;
}

void main() {
    vec3 baseAlbedo = vec3(1.0f);
    vec3 normalMapSample = vec3(0.5f, 0.5f, 1.0f);
    vec3 ao_rough_metal = vec3(0.0f, 0.6f, 1.0f);

    if (fs_in.use_triplanar != 0) {
        vec3 blendWeights = abs(fs_in.TBN[2]);
        blendWeights = (blendWeights - 0.2) * 7.0;
        blendWeights = max(blendWeights, 0.0);
        blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z);

        float distToCam = length(camera.pos - fs_in.FragPos);
        float blendCloseMid = smoothstep(3.0, 6.0, distToCam);
        float blendMidFar = smoothstep(15.0, 25.0, distToCam);
        float blendCloseFar = smoothstep(3.0, 10.0, distToCam);

        float baseScale = (1.0f / fs_in.use_triplanar);
        float scaleClose = baseScale * 4.0f;
        float scaleMid   = baseScale * 2.0f;
        float scaleFar   = baseScale * 1.0f;

        float layer = float(fs_in.material_id);

        baseAlbedo = vec3(0.99f);
        ao_rough_metal = vec3(1.0f, 0.85f, 0.0f);
        if (fs_in.material_id >= 11) {
            // skyrim
            vec3 tc = vec3(fs_in.TexCoords.xy, float(fs_in.material_id - 11));
            tc.y = -tc.y;
            baseAlbedo = texture(uAlbedoSkyrim, tc).rgb;
        } else {
            if (uTriplanarSize == 2) {
                if (fs_in.material_id == 2) {
                    // wall clay
                    baseAlbedo = SampleTriplanar2Level(uAlbedo, fs_in.FragPos, blendWeights, layer, scaleClose, scaleFar, blendCloseFar);
                    normalMapSample = SampleTriplanar2Level(uNormal, fs_in.FragPos, blendWeights, layer, scaleClose, scaleFar, blendCloseFar);
                    ao_rough_metal = SampleTriplanar2Level(uAoRoughMetal, fs_in.FragPos, blendWeights, layer, scaleClose, scaleFar, blendCloseFar);
                } else {
                    float noise = texture(uPerlin, fs_in.FragPos.xz * 0.03).r;
                    float slope = normalize(fs_in.Normal.xyz).y;
                    slope += (noise - 0.5) * 1.0925; // Adjust 0.25 to control how wild/random the transition is
                    float grassWeight = smoothstep(0.7, 0.8, slope);
                    float rockWeight  = 1.0 - smoothstep(0.3, 0.4, slope);
                    float sandWeight  = clamp(1.0 - (grassWeight + rockWeight), 0.0, 1.0);
                    float totalWeight = grassWeight + sandWeight + rockWeight;
                    grassWeight /= totalWeight;
                    sandWeight  /= totalWeight;
                    rockWeight  /= totalWeight;

                    float layerGrass = 5.0;
                    float layerSand  = 0.0;
                    float layerRock  = 1.0;

                    vec3 albedoGrass = SampleTriplanar2Level(uAlbedo, fs_in.FragPos, blendWeights, layerGrass, scaleClose, scaleFar, blendCloseFar);
                    vec3 albedoSand  = SampleTriplanar2Level(uAlbedo, fs_in.FragPos, blendWeights, layerSand,  scaleClose, scaleFar, blendCloseFar);
                    vec3 albedoRock  = SampleTriplanar2Level(uAlbedo, fs_in.FragPos, blendWeights, layerRock,  scaleClose, scaleFar, blendCloseFar);
                    baseAlbedo = (albedoGrass * grassWeight) +
                    (albedoSand  * sandWeight)  +
                    (albedoRock  * rockWeight);

                    if (distToCam < 1.0f) {
                        vec3 normGrass = SampleTriplanar2Level(uNormal, fs_in.FragPos, blendWeights, layerGrass, scaleClose, scaleFar, blendCloseFar);
                        vec3 normSand  = SampleTriplanar2Level(uNormal, fs_in.FragPos, blendWeights, layerSand,  scaleClose, scaleFar, blendCloseFar);
                        vec3 normRock  = SampleTriplanar2Level(uNormal, fs_in.FragPos, blendWeights, layerRock,  scaleClose, scaleFar, blendCloseFar);
                        normalMapSample = (normGrass * grassWeight) +
                        (normSand  * sandWeight)  +
                        (normRock  * rockWeight);

                        vec3 armGrass = SampleTriplanar2Level(uAoRoughMetal, fs_in.FragPos, blendWeights, layerGrass, scaleClose, scaleFar, blendCloseFar);
                        vec3 armSand  = SampleTriplanar2Level(uAoRoughMetal, fs_in.FragPos, blendWeights, layerSand,  scaleClose, scaleFar, blendCloseFar);
                        vec3 armRock  = SampleTriplanar2Level(uAoRoughMetal, fs_in.FragPos, blendWeights, layerRock,  scaleClose, scaleFar, blendCloseFar);
                        ao_rough_metal = (armGrass * grassWeight) +
                        (armSand  * sandWeight)  +
                        (armRock  * rockWeight);
                    } else {
                        ao_rough_metal.g = 0.9f;
                        ao_rough_metal.b = 0.1f;
                    }
                }
            } else if (uTriplanarSize == 3) {
                baseAlbedo      = SampleTriplanar3Level(uAlbedo,       fs_in.FragPos, blendWeights, layer, scaleClose, scaleMid, scaleFar, blendCloseMid, blendMidFar);
                normalMapSample = SampleTriplanar3Level(uNormal,       fs_in.FragPos, blendWeights, layer, scaleClose, scaleMid, scaleFar, blendCloseMid, blendMidFar);
                ao_rough_metal  = SampleTriplanar3Level(uAoRoughMetal, fs_in.FragPos, blendWeights, layer, scaleClose, scaleMid, scaleFar, blendCloseMid, blendMidFar);
            } else {
                vec3 tc = vec3(fs_in.TexCoords, float(fs_in.material_id));
                normalMapSample = texture(uNormal, tc).rgb;
                ao_rough_metal = texture(uAoRoughMetal, tc).rgb;
                baseAlbedo = texture(uAlbedo, tc).rgb;
            }
            if (fs_in.material_id == 2) {
                baseAlbedo = baseAlbedo * 0.2f + 0.9f;
            }
        }
    } else {
        vec3 tc = vec3(fs_in.TexCoords, float(fs_in.material_id));
        if (fs_in.material_id < 6) {
            normalMapSample = texture(uNormal, tc).rgb;
            ao_rough_metal = texture(uAoRoughMetal, tc).rgb;
            baseAlbedo = texture(uAlbedo, tc).rgb * fs_in.vert_color.rgb / 255.0f;
        } else {
            ao_rough_metal = vec3(1.0f, 0.9f, 0.0f);
            if (fs_in.material_id == 7) {
                baseAlbedo = texture(uAlbedoEmbroidery1, tc).rgb;
            } else if (fs_in.material_id == 8) {
                baseAlbedo = texture(uAlbedoEmbroidery2, tc).rgb;
            } else if (fs_in.material_id == 9) {
                baseAlbedo = texture(uAlbedoEmbroidery3, tc).rgb;
            } else if (fs_in.material_id == 10) {
                baseAlbedo = texture(uAlbedoEmbroidery4, tc).rgb;
            } else {
                tc = vec3(fs_in.TexCoords, float(fs_in.material_id - 11));
                tc.y = -tc.y;
                baseAlbedo = texture(uAlbedoSkyrim, tc).rgb;
            }
        }
    }
    vec3 tangentNormal = normalMapSample * 2.0 - 1.0;
    if (fs_in.use_triplanar != 0) {
        float normalStrength = 0.15;
        tangentNormal.xy *= normalStrength;
    }
    tangentNormal = normalize(tangentNormal);
    mat3 TBN = mat3(normalize(fs_in.TBN[0]), normalize(fs_in.TBN[1]), normalize(fs_in.TBN[2]));
    gNormal = vec4(EncodeNormal(normalize(TBN * tangentNormal)), ao_rough_metal.g, ao_rough_metal.b);
    gAlbedoSpec = vec4(baseAlbedo, ao_rough_metal.r);
}