#version 460 core
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

layout(location = 3) uniform mat3 transform;
layout(location = 5) uniform float res_factor;

out vec2 texcoord;

void main() {
    vec3 position = transform * vec3(in_position, 1.0f);
    gl_Position = vec4(position.xy, -1.0f, 1.0f);
    texcoord = in_texcoord;
}