#version 460 core
layout (location = 0) in uint in_id_position;

layout (location = 0) uniform sampler2D tex_hmap;
layout (location = 1) uniform mat4 map_model;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

void main() {
    uint x = in_id_position >> 10;
    uint y = in_id_position & 1023;
    float height = texture(tex_hmap, vec2(x, y) / 1024.0f).r;
    float xpos = float(x) / 16.0f - 32.0f;
    float ypos = float(y) / 16.0f - 32.0f;
    gl_Position = camera.proj * camera.view * map_model * vec4(xpos, height, ypos, 1.0f);
}
