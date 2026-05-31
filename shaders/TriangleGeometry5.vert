#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in ivec4 in_joint;   // joint indices
layout(location = 4) in vec4 in_weight;   // joint weights

struct InstanceData {
    mat4 modelMatrix;
    vec4 modelColor;
};

layout(std430, binding = 0) buffer InstanceBuffer {
    InstanceData instances[];
};


// New: joints matrix UBO
//layout(std140, binding = 10) uniform Bones {
//    mat4 uBones[100];  // adjust MAX_BONES to your model
//} bones;

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
} vs_out;

void main() {
    mat4 skinMat =
                    mat4(1.0f);
//    in_weight.x * bones.uBones[in_joint.x] +
//    in_weight.y * bones.uBones[in_joint.y] +
//    in_weight.z * bones.uBones[in_joint.z] +
//    in_weight.w * bones.uBones[in_joint.w];

    vec4 skinned_pos = skinMat * vec4(in_vertex, 1.0);
    vec3 skinned_normal = mat3(skinMat) * in_normal;

    uint index = gl_InstanceID + gl_BaseInstance;
    mat4 model = instances[index].modelMatrix;
//    mat4 model = mat4(1.0f);
    model[0][0] = 10.0f;
    model[1][1] = 10.0f;
    model[2][2] = 10.0f;
    vec4 color = instances[index].modelColor;
    vec4 worldPos = model * skinned_pos;
    gl_Position = camera.proj * camera.view * worldPos;
    vs_out.FragPos = worldPos.xyz;
    vs_out.TexCoords = in_texcoord;
    vs_out.vert_color = color;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalMatrix * skinned_normal;
}
