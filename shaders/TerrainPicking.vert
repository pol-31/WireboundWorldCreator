#version 460 core

uniform sampler2D tex_displacement;
uniform float dmap_depth;

uniform mat4 transform;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

out vec4 in_vert_data;

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
//    vs_out.id = float(gl_InstanceID);
    vec2 offs = vec2(x, y);
    vec2 tex_coords = (vertices[gl_VertexID].xz + offs + vec2(0.5)) / 64.0;
    vec4 p = vertices[gl_VertexID] + vec4(float(x - 32), 0.0,
    float(y - 32), 0.0);
    p.y = texture(tex_displacement, tex_coords).r * dmap_depth;
    gl_Position = camera.proj * camera.view * transform * p;

    // Provoking Vertex's id (we draw triangle strip here)
    float id = float(gl_InstanceID);
    in_vert_data = p;
}
