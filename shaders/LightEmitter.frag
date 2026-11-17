#version 460 core
layout (location = 0) out vec4 FragColor;

in vec4 vert_color;
in vec2 tex_coord;

layout (location = 2) uniform sampler2DArray uAlbedo;
layout (location = 3) uniform uint material_id;

void main() {
    vec3 tc = vec3(tex_coord.xy, float(material_id - 11));
    tc.y = -tc.y;
    vec4 albedo = texture(uAlbedo, tc).rgba;
    FragColor = vec4(albedo.rgb * vert_color.rgb, albedo.a);
//    FragColor = vec4(albedo.rgb * 10.0f, albedo.a);
}