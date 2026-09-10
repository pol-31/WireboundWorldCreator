#version 460 core
in vec3 WorldPos;
in vec3 Normal;
in vec3 ViewDir;
in vec3 FragPos;

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


// --- PROCEDURAL NOISE FUNCTIONS ---

// 2D Random Hash
vec2 hash(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

// 2D Perlin Noise
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(mix(dot(hash(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
                   dot(hash(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
               mix(dot(hash(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
                   dot(hash(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

// Fractal Brownian Motion (The "Hand-Plastered" look)
float fbm(vec2 uv) {
    // starting: 0.0, 0.5, 2.0
    float f = 0.0;
    float amp = 0.5;
    float freq = 2.0; // Scale of the wobbly lime bumps

    int octaves = 4;// 4 octaves is usually enough for plaster
//    octaves = 0;

    for(int i = 0; i < octaves; i++) {
                                 f += amp * noise(uv * freq);
                                 uv *= 2.0; // frequency multiplier
                                 amp *= 0.5; // amplitude multiplier
    }
    return f * 0.5 + 0.5; // Normalize to 0.0 - 1.0
}

void main() {
    //return;
    // 1. SETUP
    vec3 viewDir = normalize(camera.pos - FragPos);
    vec3 baseNormal = normalize(Normal);

    // 2. TRIPLANAR UV GENERATION
    vec3 blendWeights = abs(baseNormal);
    blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

    // Scale controls how large the plaster details are
    float uvScale = 2.0;
    vec2 uvX = FragPos.zy * uvScale;
    vec2 uvY = FragPos.xz * uvScale;
    vec2 uvZ = FragPos.xy * uvScale;

    // Select the dominant plane for procedural UVs
    vec2 baseUV;
    if (blendWeights.x > blendWeights.y && blendWeights.x > blendWeights.z) baseUV = uvX;
    else if (blendWeights.y > blendWeights.x && blendWeights.y > blendWeights.z) baseUV = uvY;
    else baseUV = uvZ;

    // 3. TBN MATRIX CONSTRUCTION (Procedural Tangent Space)
    vec3 up = vec3(0.0, 1.0, 0.0);
    if(abs(baseNormal.y) > 0.99) up = vec3(1.0, 0.0, 0.0); // Prevent singularity looking straight up/down
    vec3 T = normalize(cross(baseNormal, up));
    vec3 B = cross(baseNormal, T);
    mat3 TBN = mat3(T, B, baseNormal);

    // 4. PARALLAX DISPLACEMENT
    float heightScale = 0.03;
    float initialHeight = fbm(baseUV);

    // Transform ViewDir into Tangent Space
    vec3 tangentViewDir = normalize(transpose(TBN) * viewDir);

    // Single-step parallax offset
    vec2 parallaxUV = baseUV - (tangentViewDir.xy / tangentViewDir.z) * (initialHeight * heightScale);

    // 5. NORMAL MAP GENERATION (Finite Differences)
    float offset = 0.01;
    float hL = fbm(parallaxUV - vec2(offset, 0.0));
    float hR = fbm(parallaxUV + vec2(offset, 0.0));
    float hD = fbm(parallaxUV - vec2(0.0, offset));
    float hU = fbm(parallaxUV + vec2(0.0, offset));

    // localNormal is in Tangent Space (Z is up)
    vec3 localNormal = normalize(vec3(hL - hR, hD - hU, 0.15));

    // Transform the bumped normal back into World Space
    vec3 finalNormal = normalize(TBN * localNormal);

    // 6. COLOR (Albedo & Ambient Occlusion)
    vec3 albedo = vec3(0.92, 0.90, 0.88); // Whitewash color

    // Use the final displaced height to add ambient dirt to crevices
    float finalHeight = fbm(parallaxUV);
    albedo *= mix(0.7, 1.0, finalHeight);

    // 7. G-BUFFER OUTPUT
    gPosition = vec4(FragPos, 1.0);
    gNormal   = vec4(-finalNormal, 1.0);

    // Output Albedo + Specular power in the Alpha channel.
    // Plaster is matte, so specular is 0.0 (or very low).
    float specular = 1.0;
    gAlbedoSpec = vec4(albedo, specular);
//    gAlbedoSpec = vec4(1.0f);
}
//
//// --- MAIN RENDER LOGIC ---
//
//void main() {
//    // 1. TRIPLANAR UV GENERATION
//    // Determine which axis we are looking at to generate 2D UVs from 3D WorldPos
//    vec3 blendWeights = abs(Normal);
//    blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z); // Normalize
//
//    vec2 uvX = WorldPos.zy;
//    vec2 uvY = WorldPos.xz;
//    vec2 uvZ = WorldPos.xy;
//
//    // Select the dominant plane for our procedural UVs
//    vec2 baseUV;
//    if (blendWeights.x > blendWeights.y && blendWeights.x > blendWeights.z) baseUV = uvX;
//    else if (blendWeights.y > blendWeights.x && blendWeights.y > blendWeights.z) baseUV = uvY;
//    else baseUV = uvZ;
//
//    // 2. PARALLAX DISPLACEMENT (Relief mapping)
//    // Plaster doesn't need deep POM raymarching. A single-step offset is incredibly fast and looks right.
//    float heightScale = 0.03; // How deep the indentations are
//    float initialHeight = fbm(baseUV);
//
//    // Create a pseudo-tangent view direction based on the flat wall normal
//    vec3 up = vec3(0.0, 1.0, 0.0);
//    if(abs(Normal.y) > 0.99) up = vec3(1.0, 0.0, 0.0);
//    vec3 T = normalize(cross(Normal, up));
//    vec3 B = cross(Normal, T);
//    mat3 TBN = mat3(T, B, Normal);
//    vec3 tangentViewDir = normalize(transpose(TBN) * ViewDir);
//
//    // Offset the UV based on height and view angle
//    vec2 parallaxUV = baseUV - (tangentViewDir.xy / tangentViewDir.z) * (initialHeight * heightScale);
//
//    // 3. NORMAL MAP GENERATION (Finite Differences)
//    // We calculate the slope of the noise to create physical PBR bumps
//    float offset = 0.01;
//    float hL = fbm(parallaxUV - vec2(offset, 0.0));
//    float hR = fbm(parallaxUV + vec2(offset, 0.0));
//    float hD = fbm(parallaxUV - vec2(0.0, offset));
//    float hU = fbm(parallaxUV + vec2(0.0, offset));
//
//    vec3 localNormal = normalize(vec3(hL - hR, hD - hU, 0.15)); // 0.15 controls bump harshness
//    vec3 finalNormal = normalize(TBN * localNormal);
//
//    // 4. COLOR & LIGHTING
//    // Traditional whitewash is slightly off-white, extremely matte/rough
//    vec3 albedo = vec3(0.92, 0.90, 0.88);
//
//    // Add some ambient dirt to the deepest crevices of the noise
//    float finalHeight = fbm(parallaxUV);
//    albedo *= mix(0.7, 1.0, finalHeight);
//
//    // Simple Lambertian Diffuse
//    float diff = max(dot(finalNormal, normalize(lightDir)), 0.0);
//    vec3 diffuse = diff * albedo;
//
//    // Plaster has almost zero specular reflection, so keep it flat ambient + diffuse
//    vec3 ambient = 0.1 * albedo;
//
//    //TODO: add finalHeight (parallaxUV)
//    gAlbedoSpec = vec4(ambient, 1.0);
//    gNormal = vec4(finalNormal, 1.0f);
//    gPosition = vec4(FragPos, 1.0f);
//}