#version 460 core

layout(std430, binding = 9) buffer Blades {
    vec4 pos[];
};

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

layout(location = 0) uniform sampler2D tex_displacement;
layout(location = 1) uniform mat4 world_mat;
layout(location = 3) uniform float sin_time;

out vec3 vNormal;
out vec2 tc;
out float vert_seed;

void main() {
    float map_scale = world_mat[0][0];
    int bladeID = gl_VertexID / 3;
    int v       = gl_VertexID % 3;

    vec3 blade_data = pos[bladeID].xyz;
    vec3 base = vec3(blade_data.x, 0.0f, blade_data.z);
    float seed = pos[bladeID].y;
    vert_seed = seed;
    float phase = (seed - 0.5f) * sin_time * 2.0f;

    vec2 tcs[3] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.5, 1.0),
    vec2(1.0, 0.0)
    );
    tc = tcs[v];

    vec3 verts[3] = vec3[](
    vec3(-0.2, 0.0, 0.0),
    vec3( 0.0, 0.3, 0.0),
    vec3( 0.2, 0.0, 0.0)
    );

//    vec3 local = verts[v] * 1.3 * pos[bladeID].w / map_scale;
//    vec3 local = verts[v] * (1.5f / map_scale);
    vec3 local = verts[v] * 1.5f / map_scale;
//    local *= vec3(1.0f, .2 * pos[bladeID].w, 1.0f);



    // wind
    local.x += sin(phase * local.y * 10.0) * 0.02;

    // billboarding
    vec3 camRight = vec3(camera.view[0][0], camera.view[1][0], camera.view[2][0]);
    vec3 camUp    = vec3(camera.view[0][1], camera.view[1][1], camera.view[2][1]);

    vec3 worldPos =
    base +
    camRight * local.x +
    camUp    * local.y;

    vec3 normal = normalize(cross(camUp, camRight));
    vNormal = normal;

    worldPos.y += texture(tex_displacement, worldPos.xz / 64.0f + 0.5f).r;
    gl_Position = camera.proj * camera.view * world_mat * vec4(worldPos, 1.0);
}
