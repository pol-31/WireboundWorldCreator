#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_tangent;
layout(location = 4) in uvec4 in_joint;   // joint indices
layout(location = 5) in vec4 in_weight;   // joint weights

layout (location = 2) uniform mat4 modelMatrix;
layout (location = 3) uniform vec4 modelColor;
layout (location = 4) uniform uint boneOffset;

layout(std430, binding = 10) buffer Bones {
    mat4 uBones[];
} bones;

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
    uint index = gl_BaseInstance;
    mat4 skinMat =
//                    mat4(1.0f);
    in_weight.x * bones.uBones[boneOffset + in_joint.x] +
    in_weight.y * bones.uBones[boneOffset + in_joint.y] +
    in_weight.z * bones.uBones[boneOffset + in_joint.z] +
    in_weight.w * bones.uBones[boneOffset + in_joint.w];
    skinMat = mat4(1.0f);

    vec4 skinned_pos = skinMat * vec4(in_vertex, 1.0);
    vec3 skinned_normal = mat3(skinMat) * in_normal;
    vec3 skinned_tangent = mat3(skinMat) * in_tangent.xyz; // Drop the W component for math

    mat4 model = modelMatrix;
    vec4 color = modelColor;
    vec4 worldPos = model * skinned_pos;
//    worldPos = skinned_pos;
    gl_Position = camera.proj * camera.view * worldPos;
    vs_out.FragPos = worldPos.xyz;
    vs_out.TexCoords = in_texcoord;
    vs_out.vert_color = color;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * skinned_tangent);
    vec3 N = normalize(normalMatrix * skinned_normal);

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * in_tangent.w;
    vs_out.TBN = mat3(T, B, N);
}
