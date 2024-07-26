#version 460 core

out vec4 color;

//uniform sampler2D tex_color;
layout (binding = 1) uniform sampler2D tex_color;
uniform sampler2D tex_occlusion;


layout(std430, binding = 2) buffer PointsBuffer {
    vec4 points[64];
};

in VS_OUT {
    vec2 tc;
    vec4 in_vert_data;
} fs_in;

void main(void) {
    color = texture(tex_color, fs_in.tc);
    color *= texture(tex_occlusion, fs_in.tc).r;
//    for (int i = 0; i < 64; ++i) {
//        if (ivec2(points[i].xz) == ivec2(fs_in.in_vert_data.xz)) {
//            color.r = 0.1f;
//            color.g = 0.9f;
//            color.b = 0.1f;
//            break;
//        }
//    }
    color.a = 1.0f;
}