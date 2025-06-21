#include "TerrainRenderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../io/Window.h"
#include "../common/ShadersBinding.h"

TerrainRenderer::TerrainRenderer(Tile& tile, const Paths& paths)
    : tile_(tile),
      shader_(paths.shader_terrain_vert, paths.shader_terrain_tesc,
              paths.shader_terrain_tese, paths.shader_terrain_frag),
      shader_picking_(paths.shader_height_map_picking_vert,
                      paths.shader_height_map_picking_frag),
      nmap_("../../ProvingGround\\cmake-build-debug\\normal_map.png", GL_RG8) {
  Init();
}

void TerrainRenderer::Render() {
#ifndef NDEBUG
  if (shader_.Update()) {
    shader_.Bind();
    glUniform1i(shader::kTerrainHeightMap, 0);
    glUniform1i(1, 1); // material (temp)
    glUniform1i(2, 2); // normal
    glUniform1i(3, 3); // ao
  }
#endif
  if(glfwGetKey(gWindow, GLFW_KEY_1)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  shader_.Bind();
  glActiveTexture(GL_TEXTURE0);

  tile_.map_terrain_height.Bind();
  glActiveTexture(GL_TEXTURE1);
  tile_.map_erosion_deposition.Bind();

  glActiveTexture(GL_TEXTURE2);
  tile_.map_terrain_normal.Bind();
  glActiveTexture(GL_TEXTURE3);
  tile_.map_terrain_occlusion.Bind();

  glBindVertexArray(vao_);

  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//TODO: fbo shoudl be bind at Interface::Draw() or somewhere else
void TerrainRenderer::RenderPicking() const {
  //TODO: remove
}

glm::vec3 TerrainRenderer::GetYPosition(int vertex_id) const {
  int coord_x = vertex_id & 1023;
  int coord_z = vertex_id >> 10;
  /*
   auto where =
       glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
   where.y = static_cast<float>(terrain_heights_[coord_x + coord_z * 1024]);
   where.y /= 64.0f; // TODO: idk why 64.0f
   */
  return {};
}

void TerrainRenderer::Init() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  // vertex data defined in shader and accessed via gl_VertexID
  GLfloat vertices[16];
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


  // Enable the vertex attribute array
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0,
                        reinterpret_cast<void*>(0));

  shader_.Bind();
  glUniform1i(shader::kTerrainHeightMap, 0);
  glUniform1i(1, 1); // material (temp)
  glUniform1i(2, 2); // normal
  glUniform1i(3, 3); // ao
  shader_picking_.Bind();
  glUniform1i(shader::kHeightMapPickingHeightMap, 0);
}
