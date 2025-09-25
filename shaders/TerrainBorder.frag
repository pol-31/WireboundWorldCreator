//#version 460 core
//in vec2 TexCoord;
//out vec4 FragColor;
//
//layout(location = 1) uniform sampler2D spriteTex;
//layout(location = 2) uniform float time;
//
//// Simple 2D pseudo-random
//float hash(vec2 p) {
//    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
//}
//
//// Smooth noise function
//float noise(vec2 p) {
//    vec2 i = floor(p);
//    vec2 f = fract(p);
//    float a = hash(i + vec2(0.0, 0.0));
//    float b = hash(i + vec2(1.0, 0.0));
//    float c = hash(i + vec2(0.0, 1.0));
//    float d = hash(i + vec2(1.0, 1.0));
//    vec2 u = f * f * (3.0 - 2.0 * f);
//    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
//}
//
//void main() {
//    vec4 texel = texture(spriteTex, TexCoord);
//
//    // Domain warping: distort coordinates by noise
//    vec2 warp = TexCoord + 0.5 * vec2(
//    noise(TexCoord * 3.0 + time * 0.3),
//    noise(TexCoord * 5.0 - time * 0.2)
//    );
//
//    // Compute pseudo-random color based on warped coordinates
//    float r = noise(warp * 7.0 + vec2(12.0, 34.0));
//    float g = noise(warp * 9.0 + vec2(56.0, 78.0));
//    float b = noise(warp * 11.0 + vec2(90.0, 12.0));
//    vec3 dynamicColor = vec3(r, g, b);
//
//    // Mix with original texture
//    FragColor = vec4(mix(texel.rgb, dynamicColor, 0.5), texel.a);
//}

#version 460 core
in vec2 TexCoord;
out vec4 FragColor;

layout(location = 1) uniform sampler2D spriteTex;
layout(location = 2) uniform float time;

void main() {
    vec2 uv = TexCoord;

    vec4 original_texel = texture(spriteTex, uv);

    // Domain warp: sample the texture itself as a displacement map
    float speed = 0.2;
    vec2 warp = vec2(
    texture(spriteTex, uv + vec2(0.5, 0.0) + time * speed).r,
    texture(spriteTex, uv + vec2(0.0, 0.5) - time * speed).g
    );

    // Shift the original coordinates by the warped amount
    vec2 uvWarped = uv + (warp - 0.5) * 0.2; // scale warp to small offset

    // Sample the main sprite with warped coords
    vec4 texel = texture(spriteTex, uvWarped);

    // Optionally mix with some animated color overlay
//    vec3 colorOverlay = vec3(
//    0.5 + 0.5 * sin(time + uvWarped.x * 5.0),
//    0.5 + 0.5 * cos(time + uvWarped.y * 5.0),
//    0.5 + 0.5 * sin(time * 0.7)
//    );

    vec3 colorOverlay = vec3(
    0.3 + 0.2 * sin(time * 4.0 + uvWarped.x * 5.0),  // weaker red
    0.8 + 0.2 * cos(time * 3.0 + uvWarped.y * 5.0),  // strong green
    0.1 + 0.1 * sin(time * 2.0 + uvWarped.x * 3.0)   // very subtle blue
    );


    vec4 finalColor = mix(texel, vec4(colorOverlay, 1.0), 0.5);
    FragColor = vec4(finalColor.rgb,
    mix(texel.a, original_texel.a,
        pow(2.0f * length(TexCoord - vec2(0.5f)), 8)));
//    FragColor = vec4(finalColor.rgb, texel.a);
}
