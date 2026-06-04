#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

layout (location = 0) uniform sampler2D tex_scene;
layout (location = 1) uniform sampler2D tex_bloom;

uniform float bloomIntensity = 1.0;
uniform float exposure = 1.0;

void main() {
    vec3 sceneColor = texture(tex_scene, TexCoords).rgb;
    vec3 bloomColor = texture(tex_bloom, TexCoords).rgb;
    vec3 mixedColor = sceneColor + (bloomColor * bloomIntensity);
    vec3 mappedColor = vec3(1.0) - exp(-mixedColor * exposure);
    mappedColor = pow(mappedColor, vec3(1.0 / 2.2));
    FragColor = vec4(mappedColor, 1.0);
}
