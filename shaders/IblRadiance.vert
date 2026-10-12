#version 460 core
layout (location = 0) in vec3 in_vertex;

out vec3 frag_texcoord;

layout (location = 1) uniform mat4 uProj;
layout (location = 2) uniform mat4 uView;

void main() {
    frag_texcoord = in_vertex;
    vec4 pos = uProj * uView * vec4(in_vertex, 1.0);
    gl_Position = pos.xyww;
}  

