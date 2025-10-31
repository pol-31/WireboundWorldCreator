#version 460 core

in vec2 vUV;
out vec4 FragColor;

layout(location = 0) uniform sampler2D hmap; // bottom value
layout(location = 1) uniform mat4 inverse_transform; // to get "from where"
layout(location = 2) uniform bool invert;
layout(location = 3) uniform bool tiling;
layout(location = 4) uniform sampler2D mesh; // mesh value

void main() {
    ivec2 size = textureSize(hmap, 0);
    ivec2 coord = ivec2(vUV * size);

    // now it's -32 to 32
    vec2 world = vec2(coord) * 64.0f - 32.0f;

    vec4 value = inverse_transform * vec4(world.x, 0.0f, world.y, 1.0);
    vec2 mesh_coord = value.xz / 64.0f + 1.0f;




    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    float height = imageLoad(hmap, coord).r;
    ivec2 size = imageSize(hmap); // power of two
    vec4 value = transform * vec4(coord.x / 16.0f - 32.0f, height, coord.y / 16.0f - 32.0f, 1.0f);
    imageStore(mesh, coord, value);

    float height = texture(mesh, mesh_coord).r;
//    float height = texelFetch(hmap, mesh_coord, 0).r;

    FragColor = vec4(height, 0.0, 0.0, 1.0);
}
