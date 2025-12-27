#version 460 core

//layout (quads, fractional_odd_spacing) in;
layout (quads, equal_spacing, ccw) in;

layout(location = 0) uniform sampler2DArray tex_displacement;
layout(location = 3) uniform vec3 scales;
layout(location = 16) uniform mat4 transform;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

in TCS_OUT {
    vec2 tc;
} tes_in[];

out TES_OUT {
    vec2 tc;
    float height;
} tes_out;

void main(void) {
    vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);
    vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);
    vec2 tc = mix(tc2, tc1, gl_TessCoord.y);
    vec4 p1 = mix(gl_in[0].gl_Position,  gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
    vec4 p = mix(p2, p1, gl_TessCoord.y);
    for (int i = 0; i < 3; ++i) {
        float scale = scales[i];
        vec3 uv = vec3(tc / scale, i);
        p += texture(tex_displacement, uv).xyz * scale;
    }
    p = transform * p;
    gl_Position = camera.proj * camera.view * p;
    tes_out.height = p.y;
    tes_out.tc = tc;
}
