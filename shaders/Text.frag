#version 460 core

in vec2 texcoord;
uniform sampler2D tex;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex, texcoord);
    FragColor = texel.rrrr;
}