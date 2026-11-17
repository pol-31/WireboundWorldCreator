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

layout (location = 0) uniform sampler2DArray uAlbedo;
void main() {
    vec3 tc = vec3(fs_in.TexCoords, float(fs_in.material_id - 11));
    tc.y = -tc.y;
    vec4 color = texture(uAlbedo, tc).rgba;
    vec3 albedo = color.rgb;
    float alpha = color.a;
    albedo *= fs_in.vert_color.rgb / 255.0f;
    vec3 N = normalize(fs_in.Normal);
    vec3 L = normalize(vec3(0.5, 1.0, 0.3));
    float NdotL = max(dot(N, L), 0.0);
    vec3 lightColor = vec3(1.0, 0.95, 0.8) * 2.0;
    vec3 ambient = albedo * 0.3;
    vec3 diffuse = albedo * NdotL * lightColor * 0.5;
    FragColor = vec4(ambient + diffuse, alpha);
}