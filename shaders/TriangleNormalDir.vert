#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

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
    vec4 FragPosLightSpace;
    vec4 vert_color;
} vs_out;

layout (location = 0) uniform mat4 lightSpaceMatrix;

void main()
{
    uint index = gl_InstanceID + gl_BaseInstance;
    mat4 model = instances[index].modelMatrix;
    vec4 color = instances[index].modelColor;

    vs_out.FragPos = vec3(model * vec4(in_vertex, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * in_normal;
    vs_out.vert_color = color;
    vs_out.TexCoords = in_texcoord;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = camera.proj * camera.view * vec4(vs_out.FragPos, 1.0);
}
