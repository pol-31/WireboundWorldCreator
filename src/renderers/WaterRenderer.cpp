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
              "../shaders/Water.tese", "../shaders/Water.frag",
              {0, 1, 2, 4, 5, 15}),
      shader_game_(
        "../shaders/game/Terrain.vert", "../shaders/game/Terrain.tesc",
        "../shaders/game/Water.tese", "../shaders/game/Water.frag",
        {0, 1, 2, 4, 5, 15}),
      tex_foam_("../assets/tex_foam.png", GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE),
      mesh_(mesh) {
  UpdateOcean({});
}

void WaterRenderer::UpdateOcean(OceanTraits traits) {
  std::cout << "Update Ocean" << std::endl;
  ocean_ = std::make_unique<Ocean>(traits);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void WaterRenderer::Render() {
  // shader_.DebugUpdate();
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
  shader_game_.DebugUpdate();
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

void WaterRenderer::BindUniforms() {
  glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{tile_.map_scale});
  glUniformMatrix4fv(16, 1, false, glm::value_ptr(model));
  ocean_->BindRenderData();
  tex_foam_.BindSampler(4);
  tile_.map_ocean_surface_.BindSampler(5);
  tile_.map_terrain_height.BindSampler(15);
}

void WaterRenderer::SetWaterColor(glm::vec4 color) {
  shader_.Bind();
  glUniform4fv(17, 1, glm::value_ptr(color));
  shader_game_.Bind();
  glUniform4fv(17, 1, glm::value_ptr(color));
}
