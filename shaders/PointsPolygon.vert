#version 460 core
layout(location = 0) in uint in_vertex_id;

layout(location = 1) uniform uint selected_id_1;
layout(location = 2) uniform uint selected_id_2;

layout(location = 0) uniform sampler2D tex_displacement;
layout(location = 7) uniform mat4 transform;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

void main() {
    const vec4 vertices[] = vec4[](
    vec4(0.5, 0.0, -0.5, 1.0),
    vec4(0.5, 0.0, 0.5, 1.0),
    vec4(-0.5, 0.0, -0.5, 1.0),
    vec4(-0.5, 0.0, 0.5, 1.0)
    );
    int y = int(in_vertex_id & uint(1023));
    int x = int(in_vertex_id >> uint(10));
    vec2 offs = vec2(x, y);

    vec2 tc = (vertices[gl_VertexID].xz + vec2(0.5)) / 64.0 + offs / 1024.0;

    vec4 p = vertices[gl_VertexID] + vec4(float(x - 512), 0.0,
    float(y - 512), 0.0) / 16.0f/** - vec4(0.5f, 0.0f, 0.5f, 0.0f)*/;

    vec2 pos = p.xz; // other components == 0.0f


    vec4 final_pos = vec4(pos.x, 0.0f, pos.y, 1.0f);
    final_pos.y = texture(tex_displacement, tc).r;
    gl_Position = camera.proj * camera.view * transform * final_pos;

//    float t = float(in_vertex_id == selected_id_1 ||
//                    in_vertex_id == selected_id_2);
//    color_vert = mix(vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), t);
}
