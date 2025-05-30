#version 460 core
layout(location = 0) in uint in_vertex_id;

out vec2 tc_vert;
out vec2 pos_xz_vert;
out vec3 color_vert;

layout(location = 1) uniform uint selected_id_1;
layout(location = 2) uniform uint selected_id_2;

void main() {
    const vec4 vertices[] = vec4[](
    vec4(0.5, 0.0, -0.5, 1.0),
    vec4(0.5, 0.0, 0.5, 1.0),
    vec4(-0.5, 0.0, -0.5, 1.0),
    vec4(-0.5, 0.0, 0.5, 1.0)
    );
    int x = int(in_vertex_id & uint(1023));
    int y = int(in_vertex_id >> uint(10));
    vec2 offs = vec2(x, y);

    tc_vert = (vertices[gl_VertexID].xz + vec2(0.5)) / 64.0 + offs / 1024.0;

    vec4 p = vertices[gl_VertexID] + vec4(float(x - 512), 0.0,
    float(y - 512), 0.0) / 16.0f/** - vec4(0.5f, 0.0f, 0.5f, 0.0f)*/;

    pos_xz_vert = p.xz; // other components == 0.0f

    float t = float(in_vertex_id == selected_id_1 ||
                    in_vertex_id == selected_id_2);
    color_vert = mix(vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), t);
}
