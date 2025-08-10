#version 460 core

in vec2 texcoord;
layout(location = 0) uniform sampler2D tex;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex, texcoord);
    FragColor = vec4(texel.rrr, 1.0f);
}