#version 460 core

layout(location = 1) uniform float dash_size;

in float v_t;
out vec4 FragColor;

void main() {
    float stipple = mod(v_t, dash_size);
    if (stipple > dash_size / 2.0f) {
        discard;
    }
    FragColor = vec4(1.0);
}
