#version 460 core
in vec3 v_normal;
in vec3 v_world_pos;
in vec2 v_texcoord;

layout (location = 1) uniform sampler2D tex_albedo;
//layout (location = 2) uniform sampler2D tex_emission;
//layout (location = 3) uniform sampler2D tex_metal_rough;
//layout (location = 4) uniform sampler2D tex_normal;
//layout (location = 5) uniform sampler2D tex_occlusion;

layout(std140, binding = 2) uniform Environment {
	vec3 sun_color;
	float _pad1;
	vec3 sun_direction;
	float wind_speed;
	vec2 wind_velocity;
	float time;
	float delta_time;
} environment;

out vec4 color;

void main() {
	float lum = max(dot(v_normal, normalize(environment.sun_direction)), 0.0f);
//	vec4 albedo = texture(tex_albedo, v_texcoord);
	vec4 albedo = vec4(1.0f);
	color = albedo * vec4((0.3f + 0.7f * lum) * environment.sun_color, 1.0f);
}