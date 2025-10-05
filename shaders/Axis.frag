#version 460 core

layout(location = 0) uniform vec3 camera_pos;
layout(location = 1) uniform float scale;

in vec3 world_pos;
out vec4 FragColor;

void main() {
    float dist_to_camera = length(camera_pos - world_pos) / 16.0f;
    float dist_to_x = abs(world_pos.x);
    float dist_to_z = abs(world_pos.z);
    float dist_to_axis = min(dist_to_x, dist_to_z);
    float height_factor = abs(camera_pos.y) / 16.0f;

    float treshold = 0.01f * scale;
    vec3 color = vec3(1.0f - step(treshold, dist_to_z),
    1.0f - step(treshold, dist_to_x), 0.0f);
    FragColor = vec4(color,
    8.0f * height_factor * (1.0f - dist_to_camera)
    * (1.0f - step(treshold, dist_to_axis)));
}
