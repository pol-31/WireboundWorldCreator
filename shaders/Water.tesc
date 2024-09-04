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
} camera;


layout(std140, binding = 1) uniform Matrices {
    mat4 transform;
    float dmap_depth;
// Adding padding to ensure 16-byte alignment as per std140 layout rules.
    float padding[3];
};

void main(void) {
    if (gl_InvocationID == 0) {
        // Step 1: transform the vertex to view space
        vec4 ViewSpacePos00 = camera.view * transform * gl_in[0].gl_Position;
        vec4 ViewSpacePos01 = camera.view * transform * gl_in[1].gl_Position;
        vec4 ViewSpacePos10 = camera.view * transform * gl_in[2].gl_Position;
        vec4 ViewSpacePos11 = camera.view * transform * gl_in[3].gl_Position;

        // Step 2: calculate the length of the view space vector to get the distance
        float Len00 = length(ViewSpacePos00.xyz);
        float Len01 = length(ViewSpacePos01.xyz);
        float Len10 = length(ViewSpacePos10.xyz);
        float Len11 = length(ViewSpacePos11.xyz);

        const float MIN_DISTANCE = 1.0f;
        const float MAX_DISTANCE = 16.0f;

        // Step 3: map the distance to [0,1]
        float Distance00 = clamp((Len00 - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float Distance01 = clamp((Len01 - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float Distance10 = clamp((Len10 - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float Distance11 = clamp((Len11 - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

        const int MIN_TESS_LEVEL = 1;
        const int MAX_TESS_LEVEL = 7;

        // Step 4: interpolate edge tessellation level based on the closest vertex
        //         on each edge
        float TessLevel0 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(Distance10, Distance00) );
        float TessLevel1 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(Distance00, Distance01) );
        float TessLevel2 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(Distance01, Distance11) );
        float TessLevel3 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(Distance11, Distance10) );

        // Step 5: set the outer edge tessellation levels
        gl_TessLevelOuter[0] = TessLevel0;
        gl_TessLevelOuter[3] = TessLevel1; // TODO: fixed here
        gl_TessLevelOuter[2] = TessLevel2;
        gl_TessLevelOuter[1] = TessLevel3; // TODO: fixed here

        // Step 6: set the inner tessellation levels
        gl_TessLevelInner[0] = max(TessLevel1, TessLevel3);
        gl_TessLevelInner[1] = max(TessLevel0, TessLevel2);
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;
}
