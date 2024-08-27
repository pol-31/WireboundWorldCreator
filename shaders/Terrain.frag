#version 460 core

out vec4 out_color;

//uniform sampler2D tex_color;
layout(location = 1) uniform sampler2D tex_occlusion;
layout(location = 2) uniform sampler2D tex_color;

in TES_OUT {
    vec2 tc;
} fs_in;

void main(void) {
    out_color = vec4(1.0f);//texture(tex_color, fs_in.tc);
    out_color *= texture(tex_occlusion, fs_in.tc).r;
//    out_color *= blend_color;
//    out_color = mix(blend_color, out_color, 0.4f);
    out_color.a = 1.0f;
}