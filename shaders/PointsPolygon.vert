#version 460 core
layout(location = 0) in uint in_vertex_id;

layout(location = 0) uniform sampler2D tex_displacement;

layout(std140, binding = 1) uniform Matrices {
    mat4 transform;
    float dmap_depth;
// Adding padding to ensure 16-byte alignment as per std140 layout rules.
    float padding[3];
};

//layout(std430, binding = 1) buffer OutBuffer {
//    uvec2 id;
//} out_buffer; //dbg

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

out vec3 color;

layout(location = 1) uniform uint selected_id_1;
layout(location = 2) uniform uint selected_id_2;

void main() {
    int x = int(in_vertex_id & uint(1023));
    int y = int(in_vertex_id >> uint(10));
    vec2 offs = vec2(x, y);

    const vec4 quad_centre_pos = vec4(0.0, 0.0, 0.0, 1.0);
    vec2 tc = (offs - vec2(0.5)) / 1024.0;
    vec4 p = quad_centre_pos + vec4(float(x - 512), 0.0,
    float(y - 512), 0.0) / 16.0f;

    // tese shader
    p.y = texture(tex_displacement, tc).r * dmap_depth;
    gl_Position = camera.proj * camera.view * transform * p;

    // color (for frag shader)
    float t = float(
    in_vertex_id == selected_id_1
    || in_vertex_id == selected_id_2);
    color = mix(vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), t);
}

