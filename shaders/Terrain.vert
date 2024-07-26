#version 460 core

uniform sampler2D tex_displacement;
uniform float dmap_depth;

uniform mat4 transform;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

out VS_OUT {
    vec2 tc;
    vec4 in_vert_data;
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
    vec4 p = vertices[gl_VertexID] + vec4(float(x - 32), 0.0,
    float(y - 32), 0.0);
    p.y = texture(tex_displacement, vs_out.tc).r * dmap_depth;
    gl_Position = camera.proj * camera.view * transform * p;

    float id = float(gl_InstanceID);
    vs_out.in_vert_data = p;
}
