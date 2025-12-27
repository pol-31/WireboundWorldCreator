#version 460 core

layout (quads, equal_spacing, ccw) in;

in TCS_OUT {
    vec2 tc;
} tes_in[];

out TES_OUT {
    vec2 tc;
} tes_out;

layout (location = 0) uniform sampler2D tex_displacement;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

layout(location = 7) uniform mat4 transform;

void main() {
    vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);
    vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);
    vec2 tc = mix(tc2, tc1, gl_TessCoord.y);
    vec4 p1 = mix(gl_in[0].gl_Position,  gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    vec4 p = mix(p2, p1, gl_TessCoord.y);
    p.y += texture(tex_displacement, tc).r;
    gl_Position = camera.proj * camera.view * transform * p;
    tes_out.tc = tc;
}
