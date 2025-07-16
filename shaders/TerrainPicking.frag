#version 460 core

out uint id;

in TES_OUT {
    vec2 tc;
} fs_in;

void main(void) {
    // from 0-1 to 0-1024
    uvec2 pos = uvec2(fs_in.tc * 1024.0f);
    id = pos.x * 1024 + pos.y;
}
