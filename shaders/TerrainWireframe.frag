#version 460 core

out vec4 out_color;

layout(location = 1) uniform vec3 color;

in TES_OUT {
    vec3 tc;
} fs_in;

void main(void) {
    out_color = vec4(color, 1.0f);
}
