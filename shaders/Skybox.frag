#version 460 core
out vec4 FragColor;

in vec3 frag_texcoord;

layout (location = 0) uniform samplerCube tex;

void main() {
    FragColor = texture(tex, frag_texcoord);
}

