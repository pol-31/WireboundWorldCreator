#version 460
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

layout(location = 3) uniform mat3 transform;

out vec2 texcoords;

void main() {
    vec3 transformedPosition = transform * vec3(in_position, 1.0f);
    gl_Position = vec4(transformedPosition.xy, 0.0f, 1.0f);
    texcoords = in_texcoord;
}