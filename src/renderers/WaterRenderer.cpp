#include "WaterRenderer.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../io/Window.h"

WaterRenderer::WaterRenderer(Tile& tile, GeoClipmaps& mesh)
    : tile_(tile),
      shader_("../shaders/Terrain.vert", "../shaders/Terrain.tesc",
              "../shaders/Water.tese", "../shaders/Water.frag"),
      shader_game_(
        "../shaders/game/Terrain.vert", "../shaders/game/Terrain.tesc",
        "../shaders/game/Water.tese", "../shaders/game/Water.frag"),
      tex_foam_("../assets/tex_foam.png", GL_RGBA),
      mesh_(mesh) {
  UpdateOcean({});
  UpdateShaders();
}

void WaterRenderer::UpdateOcean(OceanTraits traits) {
  std::cout << "Update Ocean" << std::endl;
  ocean_ = std::make_unique<Ocean>(traits);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void WaterRenderer::Render() {
  if (shader_.Update()) {
    UpdateShaders();
  }
  ocean_->Update();
  if (glfwGetKey(gWindow, GLFW_KEY_2)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  glEnable(GL_CULL_FACE);
  shader_.Bind();
  BindUniforms();
  mesh_.RenderLowPoly();
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void WaterRenderer::RenderInGame() {
  if (shader_game_.Update()) {
    UpdateShaders();
  }
  ocean_->Update();
  if (glfwGetKey(gWindow, GLFW_KEY_1)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  glEnable(GL_CULL_FACE);
  shader_game_.Bind();
  BindUniforms();
  mesh_.Render();
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void WaterRenderer::RenderRivers() {
  // TODO: cur in TerrainRenderer
}

void WaterRenderer::UpdateShaders() {
  glm::vec4 albedo = glm::vec4(0.6f, 0.7f, 0.9f, 1.0f);
  shader_.Bind();
  glUniform1i(0, 0);
  glUniform1i(1, 1);
  glUniform1i(2, 2);
  glUniform1i(4, 4);
  glUniform1i(5, 5);
  glUniform1i(15, 15);
  glUniform4fv(17, 1, glm::value_ptr(albedo));
  shader_game_.Bind();
  glUniform1i(0, 0);
  glUniform1i(1, 1);
  glUniform1i(2, 2);
  glUniform1i(4, 4);
  glUniform1i(5, 5);
  glUniform1i(15, 15);
  glUniform4fv(17, 1, glm::value_ptr(albedo));
}

void WaterRenderer::BindUniforms() {
  glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{tile_.map_scale});
  glUniformMatrix4fv(16, 1, false, glm::value_ptr(model));
  ocean_->BindRenderData();
  glActiveTexture(GL_TEXTURE4);
  tex_foam_.Bind();
  glActiveTexture(GL_TEXTURE5);
  tile_.map_ocean_surface_.Bind();
  glActiveTexture(GL_TEXTURE15);
  tile_.map_terrain_height.Bind();
}

void WaterRenderer::SetWaterColor(glm::vec4 color) {
  shader_.Bind();
  glUniform4fv(17, 1, glm::value_ptr(color));
  shader_game_.Bind();
  glUniform4fv(17, 1, glm::value_ptr(color));
}
