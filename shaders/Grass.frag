#version 460 core

in vec3 vNormal;
in vec2 tc;
in float vert_seed;

out vec4 color;

layout(location = 2) uniform sampler2D tex_grass;
layout(location = 3) uniform float sin_time;

layout(std140, binding = 2) uniform Environment {
    vec3 sun_color;
    float _pad1;
    vec3 sun_direction;
    float wind_speed;
    vec2 wind_velocity;
    float time;
    float delta_time;
} environment;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(-environment.sun_direction);

    float NdotL = max(dot(N, L), 0.0);

//    vec3 baseColor = vec3(0.2, 0.8, 0.2);
    vec4 baseColor = texture(tex_grass, tc);
    vec3 lighting  = baseColor.rgb * environment.sun_color * NdotL;
//    lighting  = baseColor.rgb * vec3(1.0f, 1.0f * vert_seed, 1.0f);
    lighting  = baseColor.rgb * 0.8;

    if (baseColor.a < 0.5) discard;
//
    color = vec4(lighting * 1.0f, baseColor.a);
}
