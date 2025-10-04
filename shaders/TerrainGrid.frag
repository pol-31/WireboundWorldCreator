#version 460 core

layout(location = 1) uniform float map_scale;
layout(location = 2) uniform vec3 camera_pos;

in vec3 world_pos;
out vec4 FragColor;

void main() {
    float scale = sqrt(map_scale);

    // coloring major lines (every 10th)
    bool majorX = int(floor(world_pos.x / scale)) % 10 == 0;
    bool majorZ = int(floor(world_pos.z / scale)) % 10 == 0;
    vec3 color = vec3(0.3); // default grey
    if (majorX || majorZ) {
        color = vec3(0.6); // brighter for major lines
    }


    float dist_to_camera = length(camera_pos - world_pos) / 16.0f;

    float thickness = mix(0.05, 0.1, pow(1 - dist_to_camera, 2));
//    float thickness = 0.05 + 0.05 * pow(1 - dist_to_camera, 2);

    // distance to nearest grid line along X and Z
    float lineX = abs(fract(world_pos.x / scale) - 0.5);
    float lineZ = abs(fract(world_pos.z / scale) - 0.5);
    float dist = min(lineX, lineZ);

    float alpha = smoothstep(thickness, 0.0, dist);
    FragColor = vec4(color, alpha * (1.0f - dist_to_camera));
}
