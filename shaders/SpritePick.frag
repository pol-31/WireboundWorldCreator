#version 460 core

in vec2 texcoord;

layout(location = 0) uniform sampler2D tex;
layout(location = 1) uniform uint id;

out uint out_id;

void main() {
//    float transparency = texture(tex, texcoord).a;
//    out_id = uint(float(id) * step(0.1f, transparency));
    out_id = id;
}
