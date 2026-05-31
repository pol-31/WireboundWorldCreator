#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 vert_color;
} fs_in;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

//layout (location = 0) uniform sampler2D texture_diffuse1;
//layout (location = 1) uniform sampler2D texture_specular1;

void main() {
    gPosition = vec4(fs_in.FragPos, 1.0f);
    gNormal = vec4(normalize(fs_in.Normal), 1.0f);
    gAlbedoSpec.rgb = fs_in.vert_color.rgb;
    gAlbedoSpec.a = 1.0f;
//    gAlbedoSpec.a = 0.5f;
}
