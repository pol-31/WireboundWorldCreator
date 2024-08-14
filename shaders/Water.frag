#version 460 core

out vec4 out_color;

in VS_OUT {
    vec2 tc;
} fs_in;

void main(void) {
    out_color = vec4(0.0f, 0.0f, 0.3f, 1.0f);
}
