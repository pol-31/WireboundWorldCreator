#version 460 core

in vec2 texcoord;
layout(location = 0) uniform sampler2D tex;

layout(location = 1) uniform float transparency;
layout(location = 2) uniform float brightness;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex, texcoord);
    FragColor = vec4(texel.rgb * brightness, texel.a * transparency);
}