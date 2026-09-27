#version 460 core
out float FragColor;

in vec2 TexCoords;

layout (location = 0) uniform sampler2D gDepth;
layout (location = 1) uniform sampler2D gNormal;
layout (location = 2) uniform sampler2D texNoise;
layout (location = 4) uniform vec3 samples[64];

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// Match screen resolution / 4 (for 4x4 noise texture)
const vec2 noiseScale = vec2(1600.0 / 4.0, 900.0 / 4.0);

// Reconstruct View Space Position directly using inverse Projection Matrix
vec3 ReconstructViewPos(vec2 uv, float depth, mat4 invProj) {
    vec4 clipSpace = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpace = invProj * clipSpace;
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
    mat4 invProj = inverse(camera.proj);

    // 1. Reconstruct current Fragment Position in View Space
    float depth = texture(gDepth, TexCoords).r;
    vec3 fragPos = ReconstructViewPos(TexCoords, depth, invProj);

    // 2. Decode World Normal and convert to VIEW SPACE
    vec4 normData = texture(gNormal, TexCoords);
    vec3 worldNormal = DecodeNormal(normData.xy);
    vec3 normal = normalize(mat3(camera.view) * worldNormal); // Convert to View Space!

    // 3. Build TBN Matrix in View Space
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i) {
        // Sample position in View Space
        vec3 samplePos = fragPos + (TBN * samples[i]) * radius;

        // Project sample from View Space to Screen Space UV
        vec4 offset = vec4(samplePos, 1.0);
        offset = camera.proj * offset;      // View to Clip
        offset.xyz /= offset.w;             // Perspective divide (-1 to 1)
        offset.xyz = offset.xyz * 0.5 + 0.5; // NDC to UV [0, 1]

        // 4. Reconstruct the NEIGHBOR'S position in View Space
        float neighborDepth = texture(gDepth, offset.xy).r;
        vec3 neighborViewPos = ReconstructViewPos(offset.xy, neighborDepth, invProj);

        // 5. Compare View Space Z (In OpenGL, negative Z is further away)
        // Check if neighbor geometry is closer to camera than sample point
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - neighborViewPos.z));
        occlusion += (neighborViewPos.z >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / float(kernelSize));
    FragColor = occlusion;
}