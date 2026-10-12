#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

const float PI = 3.14159265359;
const int MAX_LIGHTS = 8;

layout (location = 0) uniform sampler2D gNormal;      // .rg = Norm, .b = Roughness .a = Metallic
layout (location = 1) uniform sampler2D gAlbedoSpec;  // .rgb = Albedo, .a = AO
layout (location = 2) uniform sampler2D gDepth;
layout (location = 3) uniform sampler2D gSsao;
layout (location = 4) uniform mat4 uInvViewProj;
layout (location = 5) uniform int uApplyDirLights;
layout (location = 6) uniform sampler2D sunShadowMap;
layout (location = 7) uniform mat4 sunLightSpaceMatrix;
layout (location = 8) uniform vec3 sun_dir;
layout (location = 9) uniform vec3 sun_color;
layout (location = 10) uniform samplerCube uIrradianceMap;
layout (location = 11) uniform samplerCube uPrefilterMap;
layout (location = 12) uniform sampler2D uBrdfLut;
layout (location = 13) uniform int uDoIbl;
//layout (location = 14) uniform samplerCube uIrradianceMap2;
//layout (location = 15) uniform samplerCube uPrefilterMap2;
//layout (location = 16) uniform float uIblBlendFactor;

struct PointLight {
    vec3 Position;
    vec3 Color;
    float Radius;
};

uniform int lights_num;
uniform PointLight lights[MAX_LIGHTS];

uniform samplerCubeArray shadowMaps[MAX_LIGHTS];
uniform float farPlanes[MAX_LIGHTS];

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

const vec3 sampleOffsetDirections[20] = vec3[](
vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float PointShadowCalculation(float maxDist, int lightIndex, vec3 fragPos, vec3 lightPos, float NdotL) {
//    lightPos.y *= -1.0f;
//    lightPos.y += 2.0f;
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float bias = max(0.5 * (1.0 - NdotL), 0.05);
    float farPlane = farPlanes[lightIndex];
    float diskRadius = (1.0 + (currentDepth / farPlane)) / maxDist;
    float shadow = 0.0;
    int samples = 20; // sampleOffsetDirections size
    int total_done = 0;
    for (int i = 0; i < samples; i = i + 1) {
        vec3 sampleDir = fragToLight + sampleOffsetDirections[i] * diskRadius;
        float closestDepth = texture(shadowMaps[lightIndex], vec4(sampleDir, 0)).r * farPlane;
        if (currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
        ++total_done;
    }
    shadow /= float(total_done);
    return 2.0f * shadow;
}

float DirectionalShadowCalculation(vec3 fragPos, vec3 n, vec3 sunL) {
    // NORMAL OFFSET BIAS: Push the position along the normal to escape the surface
    float offsetScale = max(0.5 * (1.0 - dot(n, sunL)), 0.05);
    vec3 biasedFragPos = fragPos + n * offsetScale;

    // Multiply the BIASED position by the light matrix
    vec4 fragPosLightSpace = sunLightSpaceMatrix * vec4(biasedFragPos, 1.0f);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(sunShadowMap, 0);

    // No more bias subtraction down here!
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(sunShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / max(denom, 0.0000001); // Prevent divide by zero
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 ReconstructViewPos(vec2 uv, float depth) {
    vec4 clipSpace = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpace = uInvViewProj * clipSpace;
    return viewSpace.xyz / viewSpace.w;
}

vec2 OctWrap(vec2 v) {
    return (1.0 - abs(v.yx)) * (vec2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0));
}

vec3 DecodeNormal(vec2 enc) {
    enc = enc * 2.0 - 1.0;
    vec3 n = vec3(enc.x, enc.y, 1.0 - abs(enc.x) - abs(enc.y));
    if (n.z < 0.0) n.xy = OctWrap(n.xy);
    return normalize(n);
}

void main() {
    // 1. Unpack G-Buffer
    float depth = texture(gDepth, TexCoords).r;
    vec3 positions = ReconstructViewPos(TexCoords, depth);
    vec3 FragPos = positions;

    vec4 normData = texture(gNormal, TexCoords);
    vec3 Normal = DecodeNormal(normData.xy);
    float Metallic = normData.a;
    float Roughness = normData.b;
    Roughness = max(Roughness, 0.05);

    vec4 albedoData = texture(gAlbedoSpec, TexCoords);
    vec3 Albedo = albedoData.rgb;

    float ssao = texture(gSsao, TexCoords).r;

    float AO = albedoData.a;

    vec3 N = normalize(Normal);
    vec3 V = normalize(camera.pos - FragPos);

    // 2. Base Reflectivity
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);

    vec3 Lo = vec3(0.0); // Total accumulated light

    // 3. Lighting Loop
    for(int i = 0; i < lights_num; ++i) {
//        break;
        vec3 L = lights[i].Position - FragPos;

        // Calculate incoming radiance based on your attenuation
        float dist = length(L);
        float maxDist = lights[i].Radius;

if (dist >= maxDist) {
    continue;
}
L = normalize(L);
vec3 H = normalize(V + L);

        float factor_linear = 9.5f / maxDist;
        float factor_quadratic = 9.5f / (maxDist * maxDist);
float attenuation = 1.0 / (1.0 + factor_linear * dist + factor_quadratic * dist * dist);
        float fade = smoothstep(maxDist, maxDist * 0.8, dist);
        attenuation *= fade;

        // Radiance = Light Color * Attenuation
        vec3 radiance = lights[i].Color * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, Roughness);
        float G   = GeometrySmith(N, V, L, Roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;

        // Energy conservation
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - Metallic; // Pure metals have no diffuse

        float NdotL = max(dot(N, L), 0.0);

        // Apply your shadow calculation
        float shadow = 1.0f * PointShadowCalculation(maxDist, i, FragPos, lights[i].Position, NdotL);
//        float shadow = 1.0f * PointShadowCalculation(maxDist, i, FragPos, vec3(0.0f, 1.0f, 0.0f), NdotL);

        // Accumulate light (Notice how shadow just scales the final radiance)
        Lo += (1.0 - shadow) * (kD * Albedo / PI + specular) * radiance * NdotL;
    }

    // --- DIRECTIONAL LIGHT (THE SUN) ---
    if (uApplyDirLights == 1) {
        // L is constant for the whole scene, just inverted direction
        vec3 sunL = normalize(-sun_dir);
        vec3 sunH = normalize(V + sunL);
        vec3 sunRadiance = sun_color;
        float sunNDF = DistributionGGX(N, sunH, Roughness);
        float sunG   = GeometrySmith(N, V, sunL, Roughness);
        vec3 sunF    = fresnelSchlick(max(dot(sunH, V), 0.0), F0);
        vec3 sunNumerator    = sunNDF * sunG * sunF;
        float sunDenominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, sunL), 0.0) + 0.0001;
        vec3 sunSpecular     = sunNumerator / sunDenominator;
        vec3 sunKS = sunF;
        vec3 sunKD = vec3(1.0) - sunKS;
        sunKD *= 1.0 - Metallic;
        float sunNdotL = max(dot(N, sunL), 0.0);
        float sunShadow = 1.0f * DirectionalShadowCalculation(FragPos, N, sunL);
        Lo += (1.0 - sunShadow) * (sunKD * Albedo / PI + sunSpecular) * sunRadiance * sunNdotL;
    }


    vec3 ambient = vec3(1.0f);
    vec3 color = vec3(1.0f);

    if (uDoIbl == 1) {
//        Albedo = Albedo * vec3(0.2f);
        // Diffuse IBL
//        Roughness = 0.9f;
//        Metallic = 0.0f;
        vec3 F_ambient = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, Roughness); // Note: needs a modified Fresnel function that takes roughness
        vec3 kS_ambient = F_ambient;
        vec3 kD_ambient = 1.0 - kS_ambient;
        kD_ambient *= 1.0 - Metallic;
        vec3 irradiance = texture(uIrradianceMap, N).rgb;
        vec3 diffuseAmbient = irradiance * Albedo;
        // Specular IBL
        vec3 R = reflect(-V, N);
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(uPrefilterMap, R,  Roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf  = texture(uBrdfLut, vec2(max(dot(N, V), 0.0), Roughness)).rg;
        vec3 specularAmbient = prefilteredColor * (F_ambient * brdf.x + brdf.y);
        vec3 diffusePart = kD_ambient * diffuseAmbient * AO * ssao;
        float specularOcclusion = mix(1.0, AO * ssao, clamp(Roughness * 2.0, 0.0f, 1.0f));
        vec3 specularPart = specularAmbient * specularOcclusion;
        ambient = diffusePart + specularPart;

        //        color = vec3(1.0f);
//                color = prefilteredColor;
        color = 1.0f * ambient + 1.0f * Lo;
    } else {
        vec3 skyColor = vec3(0.1, 0.15, 0.25);   // Cool ambient from above
        vec3 groundColor = vec3(0.05, 0.03, 0.01); // Warm ambient bounced from below
        float upFactor = N.y * 0.5 + 0.5; // 0.0 to 1.0 based on surface facing
        vec3 ambientLight = mix(groundColor, skyColor, upFactor);
        ambient = ambientLight * Albedo * AO * ssao * (1.0 - Metallic);
        color = ambient * 0.01f + Lo;
//        color = vec3(0.0f);
    }
    FragColor = vec4(color, 1.0);
}

