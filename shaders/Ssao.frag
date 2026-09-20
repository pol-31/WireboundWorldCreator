#version 460 core
out float FragColor;

in vec2 TexCoords;

layout (location = 0) uniform sampler2D gPosition;
layout (location = 1) uniform sampler2D gNormal;
layout (location = 2) uniform sampler2D texNoise;
layout (location = 3) uniform vec3 samples[64];

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

const vec2 noiseScale = vec2(1600.0/4.0, 900.0/4.0);

void main()  {
    // 1. Read World Space data from G-Buffer
    vec3 worldPos = texture(gPosition, TexCoords).xyz;
    vec3 worldNormal = normalize(texture(gNormal, TexCoords).rgb);

    // 2. CONVERT TO VIEW SPACE! (Crucial step)
    vec3 fragPos = (camera.view * vec4(worldPos, 1.0)).xyz;
    vec3 normal = normalize(mat3(camera.view) * worldNormal);

    // 3. Build TBN using View Space normal
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i) {
        // kernel_sample is now in View Space!
        vec3 kernel_sample = TBN * samples[i];
        kernel_sample = fragPos + kernel_sample * radius;

        // 4. Project from View Space to Clip Space
        vec4 offset = vec4(kernel_sample, 1.0);
        offset = camera.proj * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        // 5. Read the neighboring sample's position (which is in World Space)
        vec3 sampleWorldPos = texture(gPosition, offset.xy).xyz;

        // 6. Convert that neighbor's position to View Space to get its real depth relative to the camera!
        float sampleDepth = (camera.view * vec4(sampleWorldPos, 1.0)).z;

        // 7. Compare depths (now correctly checking View Space Z)
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= kernel_sample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = occlusion;
}