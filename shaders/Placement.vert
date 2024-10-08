#version 460 core

layout (location = 0) uniform sampler2D tex_displacement;

layout(std140, binding = 1) uniform Matrices {
    mat4 transform;
    float dmap_depth;
// Adding padding to ensure 16-byte alignment as per std140 layout rules.
    float padding[3];
};

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

out VS_OUT {
    vec2 tc;
} vs_out;

// grid InstanceId_max x VertexIndexId

void main(void) {
    const vec4 vertices[] = vec4[](
    vec4(0.5, 0.0, -0.5, 1.0),
    vec4(0.5, 0.0, 0.5, 1.0),
    vec4(-0.5, 0.0, -0.5, 1.0),
    vec4(-0.5, 0.0, 0.5, 1.0)
    );
    int x = gl_InstanceID & 63;
    int y = gl_InstanceID >> 6;

    vec2 offs = vec2(x, y);
    vs_out.tc = (vertices[gl_VertexID].xz + offs + vec2(0.5)) / 64.0;
    gl_Position = vertices[gl_VertexID] + vec4(float(x - 32), 0.0,
    float(y - 32), 0.0);
//    p.y = texture(tex_displacement, vs_out.tc).r * dmap_depth;
//    gl_Position = camera.proj * camera.view * transform * p;
}
