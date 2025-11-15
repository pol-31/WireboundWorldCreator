#version 460 core
in vec3 normal;
in vec3 position;
in vec2 texcoord;

layout (location = 1) uniform sampler2D tex_albedo;
//layout (location = 2) uniform sampler2D tex_emission;
//layout (location = 3) uniform sampler2D tex_metal_rough;
//layout (location = 4) uniform sampler2D tex_normal;
//layout (location = 5) uniform sampler2D tex_occlusion;

out vec4 color;

void main() {
	vec3 sun_position = vec3(3.0f, 10.0f, -5.0f);
	vec3 sun_color = vec3(1.0f);
	float lum = max(dot(normal, normalize(sun_position)), 0.0f);
	vec4 albedo = texture(tex_albedo, texcoord);
	color = albedo * vec4((0.3f + 0.7f * lum) * sun_color, 1.0f);
	color = vec4((0.3f + 0.7f * lum) * sun_color, 1.0f);
}