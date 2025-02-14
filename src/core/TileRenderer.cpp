#include "TileRenderer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/Details.h"
#include "../common/ShadersBinding.h"
#include "../common/OpenGlUtility.h"

//#include "../io/Window.h"

TileRenderer::TileRenderer(
    const Paths& paths, const Visibility& visibility, Map& map)
    : visibility_(visibility),
      cur_tile_(map[0]),
      fences_renderer_(cur_tile_),
      objects_renderer_(cur_tile_),
      placement_renderer_(cur_tile_, paths),
      roads_renderer_(cur_tile_),
      terrain_renderer_(cur_tile_, paths),
      water_renderer_(cur_tile_, paths),
      gen_hmap_shader_("../shaders/GenTerrainHMap.comp") {
  InitMapScaleUbo();
  terrain_renderer_.UpdateNormalMap(gen_hmap_shader_);
}

void TileRenderer::UpdatePlacement() {
  placement_renderer_.UpdatePipeline();
}

void TileRenderer::Render() {
  if (visibility_.IsTerrainVisible()) {
    terrain_renderer_.Render();
  }
  if (visibility_.IsWaterVisible()) {
    water_renderer_.Render();
  }
  if (visibility_.IsRoadsVisible()) {
    roads_renderer_.Render();
  }
  if (visibility_.IsFencesVisible()) {
    fences_renderer_.Render();
  }
  if (visibility_.IsPlacementVisible()) {
    placement_renderer_.Render();
  }
  if (visibility_.IsObjectsVisible()) {
    objects_renderer_.Render();
  }
}

void TileRenderer::RenderPlacementDraw() const {
  placement_renderer_.RenderDraw();
}

void TileRenderer::RenderPickingAll() const {
  RenderPickingTerrain();
  RenderPickingWater();
  RenderPickingFences();
  RenderPickingObjects();
}

void TileRenderer::RenderPickingTerrain() const {
  if (visibility_.IsTerrainVisible()) {
    terrain_renderer_.RenderPicking();
  }
}

void TileRenderer::RenderPickingWater() const {
  if (visibility_.IsWaterVisible()) {
    water_renderer_.RenderPicking();
  }
}

void TileRenderer::RenderPickingFences() const {
  if (visibility_.IsFencesVisible()) {
    fences_renderer_.RenderPicking();
  }
}

void TileRenderer::RenderPickingObjects() const {
  if (visibility_.IsObjectsVisible()) {
    objects_renderer_.RenderPicking();
  }
}

void TileRenderer::DeInitMapScaleUbo() {
  glDeleteBuffers(1, &map_scale_ubo_);
}

void TileRenderer::UpdateScale() {
  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(map_scale_, map_scale_, map_scale_));
  utility::UpdateUbo(map_scale_ubo_, 0, sizeof(glm::mat4),
                     glm::value_ptr(transform));
}

void TileRenderer::UpScale() {
  map_scale_ *= details::kMapScaleFactor;
  UpdateScale();
}

void TileRenderer::DownScale() {
  map_scale_ /= details::kMapScaleFactor;
  UpdateScale();
}

void TileRenderer::InitMapScaleUbo() {
  // 64 for transform matrix (we modify only scale), float height map scale
  // and 3 other float for padding
  // (to ensure 16-byte alignment as per std140 layout rules)
  utility::InitUbo(map_scale_ubo_, sizeof(glm::mat4) + 4 * sizeof(float),
                   GL_DYNAMIC_DRAW, shader::kUboMapScaleBind);

  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(map_scale_, map_scale_, map_scale_));
  glBindBuffer(GL_UNIFORM_BUFFER, map_scale_ubo_);
  void* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
  if (ptr) {
    std::memcpy(ptr, glm::value_ptr(transform), sizeof(glm::mat4));
    std::memcpy(static_cast<char*>(ptr) + sizeof(glm::mat4),
                &details::kHeightMapScale, sizeof(float));
  } else {
    std::cerr << "unable to map uniform buffer for map scale" << std::endl;
  }
  glUnmapBuffer(GL_UNIFORM_BUFFER);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::vec3 TileRenderer::GetPosition(int vertex_id) {
  if (vertex_id < 1024 * 1024) {
    return terrain_renderer_.GetYPosition(vertex_id);
  } else if (vertex_id < 2 * 1024 * 1024) {
    return water_renderer_.GetYPosition(vertex_id);
  } else if (vertex_id < 3 * 1024 * 1024) {
    return fences_renderer_.GetYPosition(vertex_id);
  } else {
    return objects_renderer_.GetYPosition(vertex_id);
  }
}

void TileRenderer::UpdateOcean(OceanTraits traits) {
  water_renderer_.UpdateOcean(traits);
}
