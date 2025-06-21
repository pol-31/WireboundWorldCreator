#version 460 core

in vec2 texcoord;

layout(location = 0) uniform sampler2D tex;
layout(location = 1) uniform float transparency;
layout(location = 2) uniform float brightness;
layout(location = 4) uniform float progress;
layout(location = 6) uniform sampler2D mask;

out vec4 FragColor;

/**void main() {
    vec4 texel = texture(tex, texcoord);
    vec4 mask_texel = texture(mask, texcoord * vec2(2.0f, 4.0f));

    float mask_value = mix(0.0f, mask_texel.r, progress);

    vec3 warm = vec3(0.5f, 0.25f, 0.0f);
    float mixval = smoothstep(0.1f, 0.3f, texel.a * mask_value);
    vec3 color = mix(warm, texel.rgb, mixval);

    float trnta = mixval * transparency;

    FragColor = vec4(color * mask_value * brightness, trnta);
}*/

void main() {
    vec4 texel = texture(tex, texcoord);
    vec4 mask_texel = texture(mask, texcoord * vec2(2.0f, 4.0f));

    float tr = 1.0f;
    float mask_value = mix(0.0f, mask_texel.r, progress);

    vec3 color;
    float trnta = step(0.2f, texel.a * mask_value) * transparency;
    if (texel.a * mask_value < 0.25f) {
        color = vec3(0.5f, 0.25f, 0.0f);
    } else {
        color = texel.rgb;
    }
    FragColor = vec4(color * mask_value * brightness, trnta);
}
