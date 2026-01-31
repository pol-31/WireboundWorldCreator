#version 460 core

out vec4 out_color;

layout(location = 1) uniform sampler2D selection_mask; // GL_RED
layout(location = 2) uniform vec3 color;

in TES_OUT {
    vec3 tc;
} fs_in;

void main(void) {
    float selected = texture(selection_mask, fs_in.tc.xz).r;
    out_color = vec4(color, selected);
}
