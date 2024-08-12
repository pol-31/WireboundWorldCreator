#version 460 core

in vec2 texcoord;
uniform sampler2D tex;

uniform float transparency;
uniform float brightness;
uniform vec4 color;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex, texcoord);
//    texel *= color;
    texel *= brightness;
    FragColor = texel;
//    FragColor = texture(tex, texcoord);
}