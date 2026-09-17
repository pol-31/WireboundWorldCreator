#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

// G-Buffer with packed PBR data in the alpha channels
layout (location = 0) uniform sampler2D gPosition;    // .rgb = Pos, .a = Roughness
layout (location = 1) uniform sampler2D gNormal;      // .rgb = Norm, .a = Metallic
layout (location = 2) uniform sampler2D gAlbedoSpec;  // .rgb = Albedo, .a = AO

struct Light {
    vec3 Position;
    vec3 Color;
};

const float LightLinear = 0.09;
const float LightQuadratic = 0.032;
const float PI = 3.14159265359;

const int MAX_LIGHTS = 8;
layout (location = 3) uniform int lights_num;
layout (location = 4) uniform Light lights[MAX_LIGHTS];

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

// --- Your Existing Shadow Math ---
float ShadowCalculation(int lightIndex, vec3 fragPos, vec3 lightPos) {
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(shadowMaps[lightIndex], vec4(fragToLight, 0)).r;
    closestDepth *= farPlanes[lightIndex];
    float currentDepth = length(fragToLight);

    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

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

void main() {
    // 1. Unpack G-Buffer
    vec4 posData = texture(gPosition, TexCoords);
    vec3 FragPos = posData.rgb;
    float Roughness = posData.a;
    // Fallback if roughness is 0 (prevent pitch black artifacts)
    Roughness = max(Roughness, 0.05);

    vec4 normData = texture(gNormal, TexCoords);
    vec3 Normal = normData.rgb;
    float Metallic = normData.a;

    vec4 albedoData = texture(gAlbedoSpec, TexCoords);
    vec3 Albedo = albedoData.rgb;
    float AO = albedoData.a;

    vec3 N = normalize(Normal);
    vec3 V = normalize(camera.pos - FragPos);

    // 2. Base Reflectivity
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);

    vec3 Lo = vec3(0.0); // Total accumulated light

    // 3. Lighting Loop
    for(int i = 0; i < lights_num; ++i) {
        vec3 L = normalize(lights[i].Position - FragPos);
        vec3 H = normalize(V + L);

        // Calculate incoming radiance based on your attenuation
        float dist = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + LightLinear * dist + LightQuadratic * dist * dist);

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
        float shadow = ShadowCalculation(i, FragPos, lights[i].Position);

        // Accumulate light (Notice how shadow just scales the final radiance)
        Lo += (1.0 - shadow) * (kD * Albedo / PI + specular) * radiance * NdotL;
    }

    // 4. Ambient Lighting
    vec3 ambient = vec3(0.03) * Albedo * AO;
    vec3 color = ambient + Lo;

    // 5. Tonemapping and Gamma Correction (Mandatory for PBR)
    color = color / (color + vec3(1.0)); // Reinhard tonemapping
    color = pow(color, vec3(1.0/2.2));   // Gamma correction

    FragColor = vec4(color, 1.0);
}