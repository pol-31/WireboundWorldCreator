#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

// G-Buffer with packed PBR data in the alpha channels
layout (location = 0) uniform sampler2D gNormal;      // .rgb = Norm, .a = Metallic
layout (location = 1) uniform sampler2D gAlbedoSpec;  // .rgb = Albedo, .a = AO
layout (location = 2) uniform sampler2D gDepth;  // .rgb = Albedo, .a = AO
layout (location = 3) uniform sampler2D gSsao;  // .rgb = Albedo, .a = AO
uniform mat4 uInvViewProj;  // .rgb = Albedo, .a = AO

struct Light {
    vec3 Position;
    vec3 Color;
    float Radius;
};

const float LightLinear = 0.99;
const float LightQuadratic = 0.000032;
const float PI = 3.14159265359;

const int MAX_LIGHTS = 8;
uniform int lights_num;
uniform Light lights[MAX_LIGHTS];

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

float ShadowCalculation(float maxDist, int lightIndex, vec3 fragPos, vec3 lightPos, float NdotL) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float bias = max(0.5 * (1.0 - NdotL), 0.05);
    float farPlane = farPlanes[lightIndex];
    float diskRadius = (1.0 + (currentDepth / farPlane)) / maxDist;
    float shadow = 0.0;
    int samples = 20; // sampleOffsetDirections size
    for (int i = 0; i < samples; ++i) {
        vec3 sampleDir = fragToLight + sampleOffsetDirections[i] * diskRadius;
        float closestDepth = texture(shadowMaps[lightIndex], vec4(sampleDir, 0)).r * farPlane;
        if (currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);
    return shadow;
}

// --- Your Existing Shadow Math ---
//float ShadowCalculation(int lightIndex, vec3 fragPos, vec3 lightPos, float NdotL) {
//    vec3 fragToLight = fragPos - lightPos;
//    float closestDepth = texture(shadowMaps[lightIndex], vec4(fragToLight, 0)).r;
//    closestDepth *= farPlanes[lightIndex];
//    float currentDepth = length(fragToLight);
//    float bias = max(0.5 * (1.0 - NdotL), 0.05);
//    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
//    return shadow;
//}

// --- PBR Functions ---
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

vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// In your Light Pass / Post-Process Shader:
vec3 ReconstructViewPos(vec2 uv, float depth) {
    // Convert UV [0,1] and Depth [0,1] to Clip Space [-1,1]
    vec4 clipSpace = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpace = uInvViewProj * clipSpace;
    return viewSpace.xyz / viewSpace.w;
}

vec2 OctWrap(vec2 v) {
    return (1.0 - abs(v.yx)) * (vec2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0));
}

// Decode vec2 back to vec3
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
    // Fallback if roughness is 0 (prevent pitch black artifacts)

    vec4 albedoData = texture(gAlbedoSpec, TexCoords);
    vec3 Albedo = albedoData.rgb;

    float AmbientOcclusion = texture(gSsao, TexCoords).r;
    AmbientOcclusion *= 0.2f;
    //Albedo *= vec3(0.2 * AmbientOcclusion);

    float AO = albedoData.a;

    vec3 N = normalize(Normal);
    vec3 V = normalize(camera.pos - FragPos);

    // 2. Base Reflectivity
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);

    vec3 Lo = vec3(0.0); // Total accumulated light

    // 3. Lighting Loop
    for(int i = 0; i < lights_num; ++i) {
        //break;
        vec3 L = normalize(lights[i].Position - FragPos);
        vec3 H = normalize(V + L);

        // Calculate incoming radiance based on your attenuation
        float dist = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + LightLinear * dist + LightQuadratic * dist * dist);
        float maxDist = lights[i].Radius;
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
        float shadow = 1.0f * ShadowCalculation(maxDist, i, FragPos, lights[i].Position, NdotL);

        // Accumulate light (Notice how shadow just scales the final radiance)
        Lo += (1.0 - shadow) * (kD * Albedo / PI + specular) * radiance * NdotL;
    }

    // 4. Ambient Lighting
    //vec3 ambient = vec3(0.03) * Albedo * AO;
    vec3 skyColor = vec3(0.1, 0.15, 0.25);   // Cool ambient from above
    vec3 groundColor = vec3(0.05, 0.03, 0.01); // Warm ambient bounced from below
    float upFactor = N.y * 0.5 + 0.5; // 0.0 to 1.0 based on surface facing
    vec3 ambientLight = mix(groundColor, skyColor, upFactor);// Remove the Albedo *= line entirely!

    // Later, at the bottom of main():
    vec3 ambient = ambientLight * Albedo * AO * AmbientOcclusion * (1.0 - Metallic);

    vec3 color = ambient + Lo;
//    color *= 0.99f;

    // 5. Tonemapping and Gamma Correction (Mandatory for PBR)

    // At the end of main():
    color = ACESFilm(color);
    color = pow(color, vec3(1.0/2.2));   // Gamma correction

//    color = vec3(pow(depth, 500.0f));
//    color = vec3(Normal);

    FragColor = vec4(color, 1.0);
}