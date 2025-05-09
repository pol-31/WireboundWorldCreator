#version 460 core
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

layout(location = 3) uniform mat3 transform;
layout(location = 5) uniform float res_factor;

out vec2 texcoord;

void main() {

    const vec2 vertices[] = vec2[](
    vec2(0.5, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, -0.5),
    vec2(-0.5, 0.5)
    );
    // we discard any pos at in_position simply ignoring it
//        vec2 position = vertices[gl_VertexID].xy;

    // vec2(res_factor, 1.0f) is cur RESOLUTION factor
    vec3 transformedPosition = transform * vec3(in_position * vec2(res_factor, 1.0f), 1.0f);
    gl_Position = vec4(transformedPosition.xy, -1.0f, 1.0f);
//    gl_Position = vec4(in_position, -1.0f, 1.0f);
    texcoord = in_texcoord;
}