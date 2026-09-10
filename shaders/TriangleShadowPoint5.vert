#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_tangent;
layout(location = 4) in uvec4 in_joint;   // joint indices
layout(location = 5) in vec4 in_weight;   // joint weights

layout (location = 2) uniform mat4 modelMatrix;
layout (location = 3) uniform vec4 modelColor;
layout (location = 4) uniform int boneOffset;

layout(std430, binding = 10) buffer Bones {
    mat4 uBones[];
} bones;

layout(location = 7) uniform mat4 u_LightViewProj;

out vec3 v_WorldPos;

void main() {
    uint index = gl_BaseInstance;
    vec4 color = modelColor;
    mat4 skinMat = mat4(1.0f);
    if (boneOffset != -1) {
        uint boneOffsetu = uint(boneOffset);
        skinMat =
        in_weight.x * bones.uBones[boneOffset + in_joint.x] +
        in_weight.y * bones.uBones[boneOffset + in_joint.y] +
        in_weight.z * bones.uBones[boneOffset + in_joint.z] +
        in_weight.w * bones.uBones[boneOffset + in_joint.w];
    }
    vec4 skinned_pos = skinMat * vec4(in_vertex, 1.0);
    mat4 model = modelMatrix;
    vec4 worldPos = model * skinned_pos;
    v_WorldPos = worldPos.xyz;
    gl_Position = u_LightViewProj * worldPos;
}
