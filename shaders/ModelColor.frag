#version 460 core
in vec3 v_normal;
in vec3 v_world_pos;
in vec2 v_texcoord;

layout (location = 1) uniform vec3 model_color;

layout(std140, binding = 2) uniform Environment {
	vec3 sun_color;
	float _pad1;
	vec3 sun_direction;
	float wind_speed;
	vec2 wind_velocity;
	float time;
	float delta_time;
} environment;

out vec4 out_color;

void main() {
	float lum = max(dot(v_normal, normalize(environment.sun_direction)), 0.0f);
	out_color = vec4(model_color * (0.3f + 0.7f * lum) * environment.sun_color, 1.0f);
}