#version 460 core

out vec4 out_color;

layout(location = 1) uniform sampler2D tex_color;  // Diffuse color texture
layout(location = 2) uniform sampler2D tex_nmap;   // Normal map (GL_RG)
layout(location = 3) uniform sampler2D tex_occlusion;   // Normal map (GL_RG)

layout(location = 4) uniform vec3 sun_direction = vec3(0.7, -0.1, -0.9);
layout(location = 5) uniform vec3 sun_color = vec3(0.5, 0.75, 0.3);
layout(location = 6) uniform vec3 ambient_color = vec3(0.1, 0.01, 0.15);

in TES_OUT {
    vec2 tc;  // Texture coordinates
} fs_in;

void main(void) {
    vec3 color = texture(tex_color, fs_in.tc).rgb * 0.9f;
    vec2 nmap_rg = texture(tex_nmap, fs_in.tc).rg;
    vec3 normal = vec3(nmap_rg * 2.0 - 1.0, sqrt(1.0 - dot(nmap_rg * 2.0 - 1.0, nmap_rg * 2.0 - 1.0)));
    normal = normalize(normal);
    float diffuse_factor = max(dot(normal, normalize(-sun_direction)), 0.0);
    vec3 lighting = ambient_color + sun_color * diffuse_factor;
    float occlusion = texture(tex_occlusion, fs_in.tc).r;
    out_color = vec4(color * lighting * occlusion, 1.0f);
}
