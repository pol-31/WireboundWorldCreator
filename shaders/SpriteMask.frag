#version 460 core

in vec2 texcoord;

layout(location = 0) uniform sampler2D tex;
layout(location = 1) uniform float transparency;
layout(location = 2) uniform float brightness;
layout(location = 4) uniform float progress;
layout(location = 6) uniform sampler2D mask;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex, texcoord);
    vec4 mask_texel = texture(mask, texcoord * 4.0f);

    vec4 final_texel = mix(mask_texel * texel, texel, progress);
    FragColor = vec4(final_texel.rgb * brightness, final_texel.a * transparency);
}
