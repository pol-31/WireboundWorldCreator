#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

layout (location = 0) uniform sampler2D tex_scene;
layout (location = 1) uniform sampler2D tex_bloom;

layout (location = 2) uniform float bloomIntensity = 1.0;
layout (location = 3) uniform float exposure = 1.0;
layout (location = 4) uniform vec2 uTexelSize; // NEW: Must pass vec2(1.0 / screenWidth, 1.0 / screenHeight) from C++

// FXAA Settings
const float FXAA_REDUCE_MIN = 1.0 / 128.0;
const float FXAA_REDUCE_MUL = 1.0 / 8.0;
const float FXAA_SPAN_MAX = 8.0;

//vec3 ACESFilm(vec3 x) {
//    float a = 2.51;
//    float b = 0.03;
//    float c = 2.43;
//    float d = 0.59;
//    float e = 0.14;
//    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
//}
//    color = ACESFilm(color);
//    color = pow(color, vec3(1.0/2.2));   // Gamma correction

void main() {
    // 1. FXAA Edge Detection Samples
    vec3 rgbNW = texture(tex_scene, TexCoords + vec2(-1.0, -1.0) * uTexelSize).rgb;
    vec3 rgbNE = texture(tex_scene, TexCoords + vec2(1.0, -1.0) * uTexelSize).rgb;
    vec3 rgbSW = texture(tex_scene, TexCoords + vec2(-1.0, 1.0) * uTexelSize).rgb;
    vec3 rgbSE = texture(tex_scene, TexCoords + vec2(1.0, 1.0) * uTexelSize).rgb;
    vec3 rgbM  = texture(tex_scene, TexCoords).rgb;

    // Convert colors to Luma (grayscale brightness) to find contrast edges
    vec3 lumaCoef = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, lumaCoef);
    float lumaNE = dot(rgbNE, lumaCoef);
    float lumaSW = dot(rgbSW, lumaCoef);
    float lumaSE = dot(rgbSE, lumaCoef);
    float lumaM  = dot(rgbM,  lumaCoef);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    // 2. Calculate edge direction
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * uTexelSize;

    // 3. Sample along the edge to blur it
    vec3 rgbA = (1.0/2.0) * (
    texture(tex_scene, TexCoords + dir * (1.0/3.0 - 0.5)).rgb +
    texture(tex_scene, TexCoords + dir * (2.0/3.0 - 0.5)).rgb);

    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
    texture(tex_scene, TexCoords + dir * (0.0/3.0 - 0.5)).rgb +
    texture(tex_scene, TexCoords + dir * (3.0/3.0 - 0.5)).rgb);

    float lumaB = dot(rgbB, lumaCoef);

    // 4. Choose the best anti-aliased color
    vec3 finalSceneColor;
    if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
        finalSceneColor = rgbA; // Fallback if blur went out of bounds
    } else {
        finalSceneColor = rgbB;
    }

    // 5. Apply Bloom (on top of the smoothed scene)
    vec3 bloomColor = texture(tex_bloom, TexCoords).rgb;
    vec3 mixedColor = finalSceneColor + (bloomColor * bloomIntensity * 1.0f);

    // 6. Tonemapping and Gamma Correction
    // (Reminder: Make sure you deleted ACES and pow() from your Light Pass!)
    vec3 mappedColor = vec3(1.0) - exp(-mixedColor * exposure * 1.0f);
    mappedColor = pow(mappedColor, vec3(1.0 / 2.2));

    FragColor = vec4(mappedColor, 1.0);
}