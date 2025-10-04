#version 460 core

//layout(location = 1) uniform float map_scale;
layout(location = 2) uniform vec3 camera_pos;
layout(location = 3) uniform vec3 color;

in vec3 world_pos;
out vec4 FragColor;

void main() {
    float dist_to_camera = length(camera_pos - world_pos) / 16.0f;
    float alpha = smoothstep(0.0f, 1.0f, 1.0f - dist_to_camera);
    FragColor = vec4(color, alpha);
}
