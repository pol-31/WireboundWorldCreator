#version 460 core

in vec2 texcoord;
layout(location = 0) uniform sampler2D tex;

layout(location = 1) uniform float transparency;
layout(location = 2) uniform float brightness;
// 3 for transform mat in vert
layout (location = 4) uniform float progress;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex, texcoord);
    float epsilon = 0.01f;
    float visibility = step(progress + epsilon, texel.a);
    FragColor = vec4(texel.rgb * brightness, visibility * transparency);
}