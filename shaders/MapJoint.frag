#version 460 core

out vec4 FragColor;

void main() {
    vec3 joint_color = vec3(1.0f);
    FragColor = vec4(joint_color, 1.0f);
}