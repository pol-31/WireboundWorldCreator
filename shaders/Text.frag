#version 460 core

in vec2 texcoord;
in vec4 color;

layout(location = 0) uniform sampler2D tex;

out vec4 FragColor;

void main() {
//    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//    vec3 darkGold = vec3(0.72, 0.53, 0.04);
//    vec3 smtAverage = vec3(0.64, 0.46, 0.05);
//    vec3 brown = vec3(0.2118, 0.1647, 0.0745);
//    vec3 darkBrownGold = vec3(0.55, 0.40, 0.05);
    float bitmap_value = texture(tex, texcoord).r;
    FragColor = vec4(bitmap_value * 1.1f / 255.0f) * color;
//    FragColor = vec4(1.0f);
}
