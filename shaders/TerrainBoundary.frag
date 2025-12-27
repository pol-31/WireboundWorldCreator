#version 460 core

layout(location = 1) uniform float map_scale;
layout(location = 2) uniform vec3 camera_pos;

in vec3 world_pos;
out vec4 FragColor;

void main() {
    float halfSizeOuter = 32.5 * map_scale;
    float thickness     = 0.5  * map_scale;   // outline width
    float halfSizeInner = halfSizeOuter - thickness;

    // Signed distances to outer / inner squares (XZ)
    float dOuter = max(abs(world_pos.x), abs(world_pos.z)) - halfSizeOuter;
    float dInner = halfSizeInner - max(abs(world_pos.x), abs(world_pos.z));

    // Soft edges
    float aa = 0.5 * map_scale;

    float outerMask = smoothstep(aa, 0.0, dOuter);
    float innerMask = smoothstep(aa, 0.0, dInner);

    // Ring = inside outer AND outside inner
    float alpha = outerMask * innerMask;

    // Camera fade
    float dist_to_camera = length(camera_pos - world_pos) / 16.0;
    alpha *= (1.0 - dist_to_camera * dist_to_camera);

    FragColor = vec4(1.0, 1.0, 1.0, clamp(alpha, 0.0, 1.0));
}
