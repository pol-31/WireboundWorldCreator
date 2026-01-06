#include "WaterRenderer.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../io/Window.h"

WaterRenderer::WaterRenderer(Tile& tile)
    : tile_(tile),
      shader_("../shaders/Water.vert", "../shaders/Water.tesc",
              "../shaders/Water.tese", "../shaders/Water.frag"),
      tex_foam_("../assets/tex_foam.png", GL_RGBA) {
  Init();
}

void WaterRenderer::UpdateOcean(OceanTraits traits) {
  std::cout << "Update Ocean" << std::endl;
  ocean_ = std::make_unique<Ocean>(traits);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void WaterRenderer::Render() {
#ifndef NDEBUG
  if (shader_.Update()) {
    shader_.Bind();
    glUniform1i(0, 0);
    glUniform1i(1, 1);
    glUniform1i(2, 2);
    glUniform1i(4, 4);
    glUniform1i(15, 15);
    glm::vec4 albedo = glm::vec4(0.6f, 0.7f, 0.9f, 1.0f);
    glUniform4fv(17, 1, glm::value_ptr(albedo));
  }
#endif
  ocean_->Update();
  if (glfwGetKey(gWindow, GLFW_KEY_2)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  shader_.Bind();
  glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{tile_.map_scale});
  glUniformMatrix4fv(16, 1, false, glm::value_ptr(model));
  ocean_->BindRenderData();
  glActiveTexture(GL_TEXTURE4);
  tex_foam_.Bind();
  glActiveTexture(GL_TEXTURE15);
  tile_.map_terrain_height.Bind();

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  RenderRivers();

  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void WaterRenderer::RenderRivers() {
  // TODO: cur in TerrainRenderer
}

void WaterRenderer::Init() {
  UpdateOcean({});  // TODO: no data by default (anyway we don't use it)
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  GLuint vbos[2];
  glGenBuffers(2, vbos);
  vbo_quad_ = vbos[0];
  vbo_ids_ = vbos[1];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_);

  const float quad[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

  glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), quad, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glVertexAttribDivisor(0, 0);

  for (int i = 0; i < gGridSize * gGridSize; ++i) {
    patch_grid_[i] = i;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * patch_grid_.size(),
               patch_grid_.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(GLuint), 0);
  glVertexAttribDivisor(1, 1);

  glBindVertexArray(0);

  shader_.Update();
  shader_.Bind();
  glUniform1i(0, 0);
  glUniform1i(1, 1);
  glUniform1i(2, 2);
  glUniform1i(4, 4);
  glUniform1i(15, 15);
  glm::vec4 albedo = glm::vec4(0.6f, 0.7f, 0.9f, 1.0f);
  glUniform4fv(17, 1, glm::value_ptr(albedo));
}

void WaterRenderer::DeInit() {
  GLuint vbos[] = {vbo_quad_, vbo_ids_};
  glDeleteBuffers(2, vbos);
  glDeleteVertexArrays(1, &vao_);
}

void WaterRenderer::SetWaterColor(glm::vec4 color) {
  shader_.Bind();
  glUniform4fv(17, 1, glm::value_ptr(color));
}
