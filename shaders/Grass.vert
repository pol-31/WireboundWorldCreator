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

out vec3 vNormal;
out vec2 tc;

void main() {
    float map_scale = world_mat[0][0];
    int bladeID = gl_VertexID / 3;
    int v       = gl_VertexID % 3;

    vec3 blade_data = pos[bladeID].xyz;
    vec3 base = vec3(blade_data.x, 0.0f, blade_data.z);
    float phase = pos[bladeID].w;

    // local blade shape (Y = up)
    vec3 verts[3] = vec3[](
    vec3(-0.2, 0.0, 0.0),
    vec3( 0.0, 0.3, 0.0),
    vec3( 0.2, 0.0, 0.0)
    );
    vec2 tcs[3] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.5, 1.0),
    vec2(1.0, 0.0)
    );


    vec3 local = verts[v];

    float seed = blade_data.y + 0.01;
    local *= sqrt(seed);
//    local *= seed * 2.4f / (0.5f * map_scale);
//    local *= seed * 1.4f * max(abs(blade_data.x), abs(blade_data.z));
    tc = tcs[v];

    // wind
    local.x += sin(phase * local.y * 10.0) * 0.02;
    vec3 camRight = vec3(camera.view[0][0], camera.view[1][0], camera.view[2][0]);
    vec3 camUp    = vec3(camera.view[0][1], camera.view[1][1], camera.view[2][1]);

    float angle = (seed * 1.0f - 0.5) * 0.4;
    float s = sin(angle);
    float c = cos(angle);
    float angle1 = (seed * 2.0f - 0.5) * 0.4;
    float s1 = sin(angle1);
    float c1 = cos(angle1);

    vec3 right = camRight * c + camUp * s;
    vec3 up    = camUp    * c1 - camRight * s1;

    vec3 worldPos =
    base +
    right * local.x +
    up    * local.y;

    float terrain_height = texture(tex_displacement, worldPos.xz / 64.0f + 0.5f).r;
    worldPos.y += terrain_height;

    vec3 normal = normalize(cross(camUp, camRight));
    vNormal = normal;

    gl_Position = camera.proj * camera.view * world_mat * vec4(worldPos, 1.0);
}
