#version 460 core
layout(lines) in;
layout(line_strip, max_vertices = 256) out;

layout(location = 0) uniform sampler2D tex_displacement;
layout(location = 7) uniform mat4 transform;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

in vec2 tc_vert[];
in vec2 pos_xz_vert[];
in vec3 color_vert[];

out vec3 color_geom;

// max - 1
uniform int segments = 255;

void main() {
    vec2 tc0 = tc_vert[0];
    vec2 tc1 = tc_vert[1];

    vec2 pos0 = pos_xz_vert[0];
    vec2 pos1 = pos_xz_vert[1];

    for (int i = 0; i <= segments; ++i) {
        float t = float(i) / float(segments);

        vec2 tc = mix(tc0, tc1, t);
        vec2 pos = mix(pos0, pos1, t);

        vec4 final_pos = vec4(pos.x, 0.0f, pos.y, 1.0f);
        final_pos.y = texture(tex_displacement, tc).r;
        gl_Position = camera.proj * camera.view * transform * final_pos;
        color_geom = color_vert[0];
        EmitVertex();
    }
    EndPrimitive();
}
