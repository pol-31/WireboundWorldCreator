#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_tangent;

struct InstanceData {
    mat4 modelMatrix;
    vec4 modelColor;
};

layout(std430, binding = 0) buffer InstanceBuffer {
    InstanceData instances[];
};

void main() {
    uint index = gl_InstanceID + gl_BaseInstance;
    mat4 model = instances[index].modelMatrix;
    gl_Position = model * vec4(in_vertex, 1.0);
}
