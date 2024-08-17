#include "TerrainRenderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../io/Window.h"

TerrainRenderer::TerrainRenderer(Tile& tile, const Paths& paths)
    : tile_(tile),
      shader_(paths.shader_terrain_vert, paths.shader_terrain_tesc,
              paths.shader_terrain_tese, paths.shader_terrain_frag),
      shader_picking_(paths.shader_height_map_picking_vert,
                      paths.shader_height_map_picking_frag) {
  Init();
}

void TerrainRenderer::Render() const {
  if(glfwGetKey(gWindow, GLFW_KEY_1)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  //TODO; we still need tessellation - we want to see how it should be in game
  //TODO: render terrain here

  shader_.Bind();
  glBindVertexArray(vao_);

  glActiveTexture(GL_TEXTURE0);
  tile_.map_terrain_height.Bind();

  glActiveTexture(GL_TEXTURE2);
  tile_.map_terrain_occlusion.Bind();


  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
//  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);

  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//TODO: fbo shoudl be bind at Interface::Draw() or somewhere else
void TerrainRenderer::RenderPicking() const {
  shader_picking_.Bind();
  // TODO: 0 as an offset from Details.h
  shader_picking_.SetUniform("id_offset", static_cast<unsigned int>(0));
  glBindVertexArray(vao_);
  glActiveTexture(GL_TEXTURE0);
  tile_.map_terrain_height.Bind();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
  glBindVertexArray(0);
}

glm::vec3 TerrainRenderer::GetYPosition(int vertex_id) const {
  int coord_x = vertex_id & 1023;
  int coord_z = vertex_id >> 10;/*
   auto where =
       glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
   where.y = static_cast<float>(terrain_heights_[coord_x + coord_z * 1024]);
   where.y /= 64.0f; // TODO: idk why 64.0f*/
                                 //    return where;
  return {};
}

void TerrainRenderer::Init() {
  //    return; //TODO: we use not "empty" but fill with tex_coords and pos !!!
  // Define the vertices (not actually used, but required for the draw call)
  GLfloat vertices[] = {
      -0.5f, 0.0f, -0.5f, 1.0f,
      0.5f, 0.0f, -0.5f, 1.0f,
      -0.5f, 0.0f,  0.5f, 1.0f,
      0.5f, 0.0f,  0.5f, 1.0f
  }; // TODO: its useless

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Enable the vertex attribute array
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0,
                        reinterpret_cast<void*>(0));

  shader_.Bind();
  shader_.SetUniform("tex_displacement", 0);
  //    shader_.SetUniform("tex_color", 1);
  shader_.SetUniform("tex_occlusion", 2);

  shader_picking_.Bind();
  shader_picking_.SetUniform("tex_displacement", 0);
  //    shader_picking_.SetUniform("tex_color", 1);
  shader_picking_.SetUniform("tex_occlusion", 2);
}
