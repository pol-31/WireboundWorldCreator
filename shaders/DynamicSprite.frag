#version 460

in vec2 texcoords;

layout(location = 2) uniform sampler2D tex;

out vec4 FragColor;

void main() {
    FragColor = texture(tex, texcoords);
}