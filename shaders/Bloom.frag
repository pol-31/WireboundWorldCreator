#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout (location = 0) uniform sampler2D tex_light;

void main() {
    vec3 color = texture(tex_light, TexCoords).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.95f) // 1.0f by default
    FragColor = vec4(color, 1.0);
    else
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}