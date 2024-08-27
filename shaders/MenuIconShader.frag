#version 460 core

in vec2 texcoord;
layout(location = 0) uniform sampler2D tex;

layout(location = 1) uniform float transparency;
layout(location = 2) uniform float brightness;
layout(location = 3) uniform vec4 color;

// Value between 0.0 (normal) and 1.0 (hovered)
layout(location = 4) uniform float hover_factor;

out vec4 FragColor;

void main() {
    // Fetch the original color from the texture
    vec4 originalColor = texture(tex, texcoord);

    // Convert the original color to greyscale (luminance calculation)
    float luminance = dot(originalColor.rgb, vec3(0.299, 0.587, 0.114));
    vec3 greyColor = vec3(luminance);

    vec3 finalColor = mix(greyColor, originalColor.rgb, hover_factor);
    finalColor *= mix(0.4f, 1.0f, hover_factor);
    FragColor = vec4(finalColor, originalColor.a);
}