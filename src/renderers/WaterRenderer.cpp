#include "WaterRenderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../io/Window.h"
#include "../common/ShadersBinding.h"

WaterRenderer::WaterRenderer(Tile& tile, const Paths& paths)
    : paths_(paths),
      tile_(tile),
      environment_(paths_),
      ocean_{},
      shader_(paths_.shader_water_vert, paths_.shader_water_tesc,
              paths_.shader_water_tese, paths_.shader_water_frag),
      shader_picking_(paths_.shader_height_map_picking_vert,
                      paths_.shader_height_map_picking_frag) {
  Init();
}

void WaterRenderer::UpdateOcean(OceanTraits traits) {
//  ocean_ = std::make_unique<Ocean>(paths_, environment_, traits);
//
  ocean_ = std::make_unique<Ocean>(
      paths_,
      environment_,
      OceanTraits{
          OceanLayerTraits{10000.0f, 10000.0f, 1.0f, 0.2f, 3.3f, 0.1f, 1.0f},
          OceanLayerTraits{1000.0f, 10000.0f, 1.0f, 0.2f, 3.3f, 0.1f, 1.0f},
          OceanLayerTraits{100.0f, 10000.0f, 1.0f, 0.2f, 3.3f, 0.1f, 1.0f}});
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  
}

void WaterRenderer::Render() {
  environment_.Update();
#ifndef NDEBUG
  if (shader_.Update()) {
    shader_.Bind();
    glUniform1i(0, 0);
    glUniform1i(1, 1);
    glUniform1i(2, 2);
    glUniform1i(3, 3);
    glUniform1i(4, 4);
    glUniform1i(5, 5);
    glUniform1i(6, 6);
    glUniform1i(7, 7);
    glUniform1i(8, 8);
    glUniform1i(15, 15);
  }
#endif
  ocean_->Update();
  if(glfwGetKey(gWindow, GLFW_KEY_2)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  shader_.Bind();
  ocean_->BindRenderData();
  glActiveTexture(GL_TEXTURE15);
  tile_.map_terrain_height.Bind();

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void WaterRenderer::RenderPicking() const {
  return;
  shader_picking_.Bind();
  // TODO: 1024 * 1024 as an offset from Details.h
  glUniform1ui(shader::kHeightMapPickingIdOffset,
               static_cast<unsigned int>(1024 * 1024));
  glBindVertexArray(vao_);
  glActiveTexture(GL_TEXTURE0);
  tile_.map_water_height.Bind();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
  glBindVertexArray(0);
}

glm::vec3 WaterRenderer::GetYPosition(int vertex_id) const {
  int coord_x = vertex_id & 1023;
  int coord_z = vertex_id >> 10;
  /*auto where =
      glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
  where.y = static_cast<float>(water_heights_[coord_x + coord_z * 1024]);
  where.y /= 64.0f; // TODO: idk why 64.0f*/
  //    return where;
  return {};
}

void WaterRenderer::Init() {
  ocean_ = std::make_unique<Ocean>(
      paths_,
      environment_,
      OceanTraits{
          OceanLayerTraits{10000.0f, 10000.0f, 1.0f, 0.2f, 3.3f, 0.1f, 1.0f},
          OceanLayerTraits{1000.0f, 10000.0f, 1.0f, 0.2f, 3.3f, 0.1f, 1.0f},
          OceanLayerTraits{100.0f, 10000.0f, 1.0f, 0.2f, 3.3f, 0.1f, 1.0f}});
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

  //TODO: can we somehow WARN(!) or forbid Bind()->Update() [reverse order]?
  shader_.Update();
  shader_.Bind();
  glUniform1i(0, 0);
  glUniform1i(1, 1);
  glUniform1i(2, 2);
  glUniform1i(3, 3);
  glUniform1i(4, 4);
  glUniform1i(5, 5);
  glUniform1i(6, 6);
  glUniform1i(7, 7);
  glUniform1i(8, 8);
  glUniform1i(15, 15);
}
