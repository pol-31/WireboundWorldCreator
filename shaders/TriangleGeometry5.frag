#version 460 core
//layout (location = 0) out vec4 gPosition;
layout (location = 0) out vec4 gNormal;
layout (location = 1) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 vert_color;
    mat3 TBN;
} fs_in;

layout(std140, binding = 0) uniform Camera {
    vec3 pos;       float _pad0;
    vec3 forward;   float _pad1;
    vec3 right;     float _pad2;
    vec3 up;        float cos_half_fov;
    mat4 view;
    mat4 proj;
} camera;

layout (location = 0) uniform sampler2D texture_diffuse1;
layout (location = 1) uniform sampler2D texture_normal;

vec2 OctWrap(vec2 v) {
    return (1.0 - abs(v.yx)) * (vec2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0));
}

vec2 EncodeNormal(vec3 n) {
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    vec2 enc = n.z >= 0.0 ? n.xy : OctWrap(n.xy);
    return enc * 0.5 + 0.5;
}

void main() {
    mat3 TBN = mat3(normalize(fs_in.TBN[0]), normalize(fs_in.TBN[1]), normalize(fs_in.TBN[2]));
    vec3 normalMapSample = texture(texture_normal, fs_in.TexCoords).rgb;
    vec3 finalNormal;

    if (normalMapSample == vec3(0.0)) {
        finalNormal = TBN[2];
    } else {
        vec3 tangentNormal = normalMapSample * 2.0 - 1.0;
        finalNormal = normalize(TBN * tangentNormal);
    }
    gNormal = vec4(EncodeNormal(finalNormal), 0.9f, 0.9f);
    gAlbedoSpec.rgb = texture(texture_diffuse1, fs_in.TexCoords).rgb * fs_in.vert_color.rgb;
    gAlbedoSpec.a = 1.0f;
}
