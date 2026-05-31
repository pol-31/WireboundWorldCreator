#version 460 core

in vec4 FragPos;

layout (location = 0) uniform vec3 lightPos;
layout (location = 1) uniform float far_plane;

void main() {
    float lightDistance = length(FragPos.xyz - lightPos);
    lightDistance = lightDistance / far_plane;
    gl_FragDepth = lightDistance;
}