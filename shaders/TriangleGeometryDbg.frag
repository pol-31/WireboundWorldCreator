#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 vert_color;
    mat3 TBN;
    flat uint material_id;
    flat uint use_triplanar;
} fs_in;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

vec2 OctWrap(vec2 v) {
    return (1.0 - abs(v.yx)) * (vec2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0));
}

vec2 EncodeNormal(vec3 n) {
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    vec2 enc = n.z >= 0.0 ? n.xy : OctWrap(n.xy);
    return enc * 0.5 + 0.5;
}

void main() {
    vec3 baseAlbedo = vec3(1.0f);
    if (fs_in.use_triplanar == 0) {
        baseAlbedo *= fs_in.vert_color.rgb / 255.0f;
    } else {
        baseAlbedo *= pow(fs_in.FragPos.y, 2.0f) / 10.0f;
    }
    FragColor = vec4(baseAlbedo, 1.0f);
}