#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in ivec4 in_joint;   // joint indices
layout(location = 4) in vec4 in_weight;   // joint weights

layout (location = 0) uniform mat4 model_mat;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

// New: joints matrix UBO
layout(std140, binding = 10) uniform Bones {
    mat4 uBones[100];  // adjust MAX_BONES to your model
} bones;

void main(){
    // Skinning: linear blend
    mat4 skinMat =
    in_weight.x * bones.uBones[in_joint.x] +
    in_weight.y * bones.uBones[in_joint.y] +
    in_weight.z * bones.uBones[in_joint.z] +
    in_weight.w * bones.uBones[in_joint.w];

    vec4 skinned_pos = skinMat * vec4(in_vertex, 1.0);
    vec4 world_pos = model_mat * skinned_pos;
	gl_Position = camera.proj * camera.view * world_pos;
}
