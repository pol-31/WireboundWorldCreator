#version 460 core

layout (vertices = 4) out;

in VS_OUT {
    vec2 tc;
} tcs_in[];

out TCS_OUT {
    vec2 tc;
} tcs_out[];

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

layout(location = 16) uniform mat4 transform;

float computeQuantTess(float dist) {
    float uTessStartDist = 1.0f;
    float uTessEndDist = 8.0f;
    float uQuantLevels = 8.0f;
    float n = clamp((dist - uTessStartDist) / max(0.0001, (uTessEndDist - uTessStartDist)), 0.0, 1.0);
    float smooth_ = smoothstep(0.0, 1.0, n);
    float t = mix(16.0f, 1.0f, smooth_);
    float q = max(1.0, floor(t * uQuantLevels + 0.5) / uQuantLevels);
    return q;
}

vec2 edgeMidpointWorld(vec2 patch_xy, vec2 a, vec2 b) {
    vec2 midLocal = 0.5 * (a + b);
    return patch_xy + midLocal;
}

void main() {
    tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    float map_scale = transform[0].x;
    vec2 uCameraXY = camera.pos.xz / map_scale;

    if (gl_InvocationID == 0) {
        vec2 p = gl_in[gl_InvocationID].gl_Position.xz;

        vec2 v0 = vec2(-0.5, -0.5);
        vec2 v1 = vec2(0.5, -0.5);
        vec2 v2 = vec2(0.5, 0.5);
        vec2 v3 = vec2(-0.5, 0.5);

        vec2 mEdge0 = edgeMidpointWorld(p, v0, v1); // edge 0: v0-v1 (u outer 0)
        vec2 mEdge1 = edgeMidpointWorld(p, v1, v2); // edge 1: v1-v2 (outer 1)
        vec2 mEdge2 = edgeMidpointWorld(p, v2, v3); // edge 2: v2-v3 (outer 2)
        vec2 mEdge3 = edgeMidpointWorld(p, v3, v0); // edge 3: v3-v0 (outer 3)

        float d0 = distance(mEdge0, uCameraXY);
        float d1 = distance(mEdge1, uCameraXY);
        float d2 = distance(mEdge2, uCameraXY);
        float d3 = distance(mEdge3, uCameraXY);

        float e0 = computeQuantTess(d0);
        float e1 = computeQuantTess(d1);
        float e2 = computeQuantTess(d2);
        float e3 = computeQuantTess(d3);

        vec2 patchCenter = (vec2(p) + vec2(0.5,0.5));
        float dc = distance(patchCenter, uCameraXY);
        float inner = computeQuantTess(dc);
        inner = max(1.0, inner);

        gl_TessLevelOuter[0] = max(1.0, e0);
        gl_TessLevelOuter[1] = max(1.0, e1);
        gl_TessLevelOuter[2] = max(1.0, e2);
        gl_TessLevelOuter[3] = max(1.0, e3);

        gl_TessLevelInner[0] = inner;
        gl_TessLevelInner[1] = inner;
    }
    barrier();
}
