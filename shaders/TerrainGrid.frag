#version 460 core

layout(location = 1) uniform float map_scale;
layout(location = 2) uniform vec3 camera_pos;

in vec3 world_pos;
out vec4 FragColor;

void main() {
    float scale = 1.0f / map_scale;

    float dist_to_camera = length(camera_pos - world_pos) / 16.0f;
    float dist_to_axis = min(abs(world_pos.x), abs(world_pos.z));
    float height_factor = abs(camera_pos.y) / 16.0f;

    float lineX = abs(fract(log2(abs(world_pos.x) * scale)) - 0.5f);
    float lineZ = abs(fract(log2(abs(world_pos.z) * scale)) - 0.5f);
    float dist = min(lineX, lineZ);

    float thickness = 0.03f * height_factor;
    float alpha = 2.0f * smoothstep(thickness, 0.0, dist) * height_factor * dist_to_axis * (1.0f - pow(dist_to_camera, 2));

    vec3 color = vec3(1.0f);
    FragColor = vec4(color, smoothstep(0.0f, 1.0f, alpha));
}
