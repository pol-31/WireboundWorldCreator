#version 460 core
in vec2 TexCoord;
out vec4 FragColor;

layout(location = 1) uniform sampler2D spriteTex;

void main() {
    FragColor = texture(spriteTex, TexCoord);
}
