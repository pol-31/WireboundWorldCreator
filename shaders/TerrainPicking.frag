#version 460 core

out uint id;
flat in uint vert_id;

void main(void) {
    // Provoking Vertex's id
    id = vert_id;
}