#version 460 core

in vec2 texcoord;
uniform sampler2D tex;

uniform float transparency;
uniform float brightness;
uniform vec3 color;

out vec4 FragColor;

void main() {
//    vec3 texel = texture(tex, texcoord).xyz;
//    texel = mix(texel, color, 0.5f);
//    texel *= brightness;
//    FragColor = vec4(texel, transparency);
    FragColor = texture(tex, texcoord);
}