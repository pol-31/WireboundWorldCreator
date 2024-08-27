#version 460 core

out vec4 out_color;

layout(location = 1) uniform sampler2D tex_draw;

in TES_OUT {
    vec2 tc;
} fs_in;

void main(void) {
    out_color = vec4(1.0f);
    out_color *= texture(tex_draw, fs_in.tc).r;
    out_color.a = 1.0f;
}