#version 460 core

in vec2 texcoord;
uniform usampler2D tex;
out vec4 FragColor;

void main() {
    uvec4 tex_color = texture(tex, texcoord);
    float value = float(tex_color.r);
    float transparency = float(tex_color.a);
    FragColor = vec4(value, value, value, transparency);
}
