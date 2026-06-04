#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_tangent;

struct InstanceData {
    mat4 modelMatrix;
    vec4 modelColor;
};

layout(std430, binding = 0) buffer InstanceBuffer {
    InstanceData instances[];
};

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 vert_color;
    mat3 TBN;
} vs_out;

void main() {
    uint index = gl_InstanceID + gl_BaseInstance;
    mat4 model = instances[index].modelMatrix;
//    mat4 model = mat4(1.0f);
//    float scale = 1.0f;
//    model[0][0] = scale;
//    model[1][1] = scale;
//    model[2][2] = scale;
    vec4 color = instances[index].modelColor;
    vec4 worldPos = model * vec4(in_vertex, 1.0);
    gl_Position = camera.proj * camera.view * worldPos;
    vs_out.FragPos = worldPos.xyz;
    vs_out.TexCoords = in_texcoord;
    vs_out.vert_color = color;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * in_tangent.xyz);
    vec3 N = normalize(normalMatrix * in_normal);

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * in_tangent.w;
    vs_out.TBN = mat3(T, B, N);
}
