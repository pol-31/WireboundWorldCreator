#version 460 core

in vec3 v_WorldPos;

layout(location = 5) uniform vec3 u_LightPos;
layout(location = 6) uniform float u_FarPlane;

void main() {
    float lightDistance = length(v_WorldPos - u_LightPos);
    lightDistance = lightDistance / u_FarPlane;
    gl_FragDepth = lightDistance;
}