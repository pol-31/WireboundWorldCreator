#version 460 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec2 in_texcoord;

struct ParticleInstance {
    mat4 modelMatrix;
    vec4 color;
    float lifetime;
    float size;
    uint materialId;
    uint padding[3];
};

layout(std430, binding = 3) buffer ParticleBuffer {
    ParticleInstance instances[];
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
    vec2 TexCoords;
    vec4 color;
    flat uint materialId;
} vs_out;

void main() {
    uint index = gl_InstanceID;
    ParticleInstance p = instances[index];
    vec3 position = p.modelMatrix[3].xyz;
    float size = p.size;

    mat3 billboard = mat3(
    camera.right,
    camera.up,
    -camera.forward
    );
    vec3 worldPos = billboard * in_vertex * size + position;
    
    gl_Position = camera.proj * camera.view * vec4(worldPos, 1.0);
    
    vs_out.TexCoords = in_texcoord;
    vs_out.color = p.color;
    vs_out.materialId = p.materialId;
}
