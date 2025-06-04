#version 460 core
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

layout(location = 3) uniform mat3 transform;
layout(location = 5) uniform float res_factor;

out vec2 texcoord;

layout(location = 6) uniform mat3 transform_texcoord;

void main() {
    vec3 transformedPosition = transform * vec3(in_position * vec2(res_factor, 1.0f), 1.0f);
    gl_Position = vec4(transformedPosition.xy, -1.0f, 1.0f);
    vec3 transformed_texcoord = transform_texcoord * vec3(in_texcoord, 1.0f);
    texcoord = transformed_texcoord.xy;
}