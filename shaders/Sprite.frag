#version 460 core

in vec2 texcoord;
layout(location = 0) uniform sampler2D tex;

layout(location = 1) uniform float transparency;
layout(location = 2) uniform float brightness;
layout(location = 7) uniform vec4 color;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex, texcoord);
    texel *= color;
//    vec4 something = vec4(texel.rgb * brightness, texel.a * transparency);
    FragColor = vec4(texel.rgb * brightness, texel.a * transparency);
//    FragColor = vec4(1.0f);
}