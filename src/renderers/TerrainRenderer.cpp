#include "TerrainRenderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include "../io/Window.h"
#include "../common/ShadersBinding.h"

#include "../modes/TerrainInstanceData.h"

TerrainRenderer::TerrainRenderer(Tile& tile, const Paths& paths)
    : tile_(tile),
      shader_(
          paths.shader_terrain_vert, paths.shader_terrain_tesc,
          paths.shader_terrain_tese, paths.shader_terrain_frag),
      shader_picking_(
          paths.shader_terrain_vert, paths.shader_terrain_tesc,
          paths.shader_terrain_tese, paths.shader_terrain_picking_frag),
      nmap_("../../ProvingGround\\cmake-build-debug\\normal_map.png", GL_RG8),
      shader_selection_(
          paths.shader_terrain_vert, paths.shader_terrain_tesc,
          "../shaders/TerrainSelection.tese",
          "../shaders/TerrainSelection.frag"),
      shader_wireframe_(
          paths.shader_terrain_vert, "../shaders/TerrainWireframe.tesc",
          paths.shader_terrain_tese, "../shaders/TerrainWireframe.frag"),
      border_shader_(
          "../shaders/TerrainBorder.vert", "../shaders/TerrainBorder.frag") {
  Init();
}

void TerrainRenderer::DeInit() {
  GLuint vbos[] = {vbo_quad_, vbo_ids_, border_vbo_};
  glDeleteBuffers(3, vbos);
  GLuint vaos[] = {vao_, border_vao_};
  glDeleteVertexArrays(1, vaos);
}

void TerrainRenderer::Render() {
  if (shader_.Update()) {
    shader_.Bind();
    glUniform1i(shader::kTerrainHeightMap, 0);
    glUniform1i(1, 1); // material (temp)
    glUniform1i(2, 2); // normal
    glUniform1i(3, 3); // ao
    shader_picking_.Bind();
    glUniform1i(shader::kTerrainHeightMap, 0);
  }
#ifndef NDEBUG
  if (shader_.Update()) {
    shader_.Bind();
    glUniform1i(shader::kTerrainHeightMap, 0);
    glUniform1i(1, 1); // material (temp)
    glUniform1i(2, 2); // normal
    glUniform1i(3, 3); // ao
    shader_picking_.Bind();
    glUniform1i(shader::kTerrainHeightMap, 0);
  }
#endif
  if(glfwGetKey(gWindow, GLFW_KEY_1)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  shader_.Bind();
  glm::mat4 transform = glm::scale(glm::mat4{1.0f}, glm::vec3{tile_.map_scale});
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(transform));

  glActiveTexture(GL_TEXTURE0);

  tile_.map_terrain_height.Bind();
  glActiveTexture(GL_TEXTURE1);
  tile_.map_terrain_ao.Bind();

  glActiveTexture(GL_TEXTURE2);
  tile_.map_terrain_normal.Bind();
  glActiveTexture(GL_TEXTURE3);
  tile_.map_terrain_erosion_thermal.Bind();

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  //  glDrawArrays(GL_PATCHES, 0, patch_vertices.size());
  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  /// render SUM-layer borders
  border_shader_.Bind();
  glBindVertexArray(border_vao_);
  glm::mat4 model_mat = glm::scale(glm::mat4(1.0f), glm::vec3(tile_.map_scale * 64.0f));
  glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(model_mat));
  glEnable(GL_CULL_FACE);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glDisable(GL_CULL_FACE);
}

//TODO: remove?
void TerrainRenderer::Render(TerrainInstanceData* terrain) {
  std::cerr << "TerrainRenderer::Render(terrain) unimplemented" << std::endl;
}

void TerrainRenderer::RenderWireframe(
    TerrainInstanceData* terrain, glm::vec3 color) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_wireframe_.Bind();
  glActiveTexture(GL_TEXTURE0);
//  terrain->hmap.Bind();
  terrain->data.hmap.Bind();
  glm::mat4 transform = glm::mat4{1.0f};
  transform = glm::translate(transform, terrain->translate);
  transform = glm::rotate(
      transform, glm::length(terrain->rotate), glm::normalize(terrain->rotate));
  transform = glm::scale(transform, terrain->scale * tile_.map_scale);
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(transform));

  glUniform3fv(1, 1, glm::value_ptr(terrain->color));

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  //  glDrawArrays(GL_PATCHES, 0, patch_vertices.size());
  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::RenderSelection(
    TerrainInstanceData* terrain,
    const Texture& selection_mask, glm::vec3 color) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); /// wireframe
  shader_selection_.Bind();
  glActiveTexture(GL_TEXTURE0);
  terrain->data.hmap.Bind();
  glm::mat4 transform = glm::mat4{1.0f};
  transform = glm::translate(transform, terrain->translate);
  transform = glm::rotate(
      transform, glm::length(terrain->rotate), glm::normalize(terrain->rotate));
  transform = glm::scale(transform, terrain->scale * tile_.map_scale);
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(transform));
  // NOT terrain->hmap.Bind();
  glActiveTexture(GL_TEXTURE1);
  selection_mask.Bind();
  glUniform3fv(2, 1, glm::value_ptr(color));

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  //  glDrawArrays(GL_PATCHES, 0, patch_vertices.size());
  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//TODO: fbo shoudl be bind at Interface::Draw() or somewhere else
void TerrainRenderer::RenderPicking() const {
  shader_picking_.Bind();
  glm::mat4 transform = glm::scale(glm::mat4{1.0f}, glm::vec3{tile_.map_scale});
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(transform));
  glActiveTexture(GL_TEXTURE0);
  tile_.map_terrain_height.Bind();
  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
  glBindVertexArray(0);
}

void TerrainRenderer::RenderPicking(TerrainInstanceData* terrain) const {
  shader_picking_.Bind();
  glActiveTexture(GL_TEXTURE0);
  terrain->data.hmap.Bind();
  glm::mat4 transform = glm::mat4{1.0f};
  transform = glm::translate(transform, terrain->translate);
  transform = glm::rotate(
      transform, glm::length(terrain->rotate), glm::normalize(terrain->rotate));
  transform = glm::scale(transform, terrain->scale * tile_.map_scale);
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(transform));
  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
  glBindVertexArray(0);
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

void TerrainRenderer::UpdateTransformUniform(glm::mat4 mat) {
  shader_.Bind();
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(mat));
//  shader_picking_;
//  shader_selection_;
//  shader_wireframe_;
}

void TerrainRenderer::Init() {
  GLuint vaos[2];
  glGenVertexArrays(2, vaos);
  vao_ = vaos[0];
  border_vao_ = vaos[1];
  glBindVertexArray(vao_);

  GLuint vbos[3];
  glGenBuffers(3, vbos);
  vbo_quad_ = vbos[0];
  vbo_ids_ = vbos[1];
  border_vbo_ = vbos[2];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_);

  const float quad[] = {
      0.0f, 0.0f,
      1.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f
  };

  glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), quad, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glVertexAttribDivisor(0, 0);

  for (int i = 0; i < gGridSize * gGridSize; ++i) {
    patch_grid_[i] = i;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * patch_grid_.size(), patch_grid_.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(GLuint), 0);
  glVertexAttribDivisor(1, 1);

  glBindVertexArray(0);

  const float cube[] = {
      // back
      -0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f,  0.5f, -0.5f,
      0.5f,  0.5f, -0.5f,
      -0.5f,  0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,

      // front
      -0.5f, -0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,
      0.5f, -0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,
      -0.5f, -0.5f,  0.5f,
      -0.5f,  0.5f,  0.5f,

      // left
      -0.5f,  0.5f,  0.5f,
      -0.5f, -0.5f, -0.5f,
      -0.5f,  0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,
      -0.5f,  0.5f,  0.5f,
      -0.5f, -0.5f,  0.5f,

      // right
      0.5f,  0.5f,  0.5f,
      0.5f,  0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,

      // bottom
      -0.5f, -0.5f, -0.5f,
      0.5f, -0.5f,  0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f,  0.5f,
      -0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f,  0.5f,

      // top
      -0.5f,  0.5f, -0.5f,
      0.5f,  0.5f, -0.5f,
      0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,
      -0.5f,  0.5f,  0.5f,
      -0.5f,  0.5f, -0.5f,
  };

  glBindVertexArray(border_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, border_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  shader_.Bind();
  glUniform1i(shader::kTerrainHeightMap, 0);
  glUniform1i(1, 1); // material (temp)
  glUniform1i(2, 2); // normal
  glUniform1i(3, 3); // ao
  shader_selection_.Bind();
  glUniform1i(shader::kTerrainHeightMap, 0);
  glUniform1i(1, 1); // selection mask
  shader_wireframe_.Bind();
  glUniform1i(shader::kTerrainHeightMap, 0);
  shader_picking_.Bind();
  glUniform1i(shader::kHeightMapPickingHeightMap, 0);
}
