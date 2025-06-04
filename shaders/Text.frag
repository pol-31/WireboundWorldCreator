#version 460 core

in vec2 texcoord;
layout(location = 0) uniform sampler2D tex;

layout(location = 1) uniform float transparency;
layout(location = 2) uniform float brightness;

layout(location = 4) uniform vec3 color;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex, texcoord);
//    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec3 darkGold = vec3(0.72, 0.53, 0.04);
    vec3 smtAverage = vec3(0.64, 0.46, 0.05);
    vec3 brown = vec3(0.2118, 0.1647, 0.0745);
    vec3 darkBrownGold = vec3(0.55, 0.40, 0.05);

    FragColor = vec4(color * texel.r * brightness, texel.r * transparency);
}
