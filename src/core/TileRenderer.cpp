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

//#include "../io/Window.h"

void TileRenderer::LoadMap(std::string_view world_map) {
  std::ifstream file(world_map.data());
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << world_map << std::endl;
    return;
  }
  TileInfo tile_info;
  auto tile_info_map = tile_info.CreateUnorderedMap();

  std::string line;
  while (std::getline(file, line)) {
    size_t delimiterPos = line.find('=');
    if (delimiterPos != std::string::npos) {
      // "=" means end of current tile
      if (delimiterPos == 0) {
        tiles_info_.push_back(tile_info);
        tile_info = TileInfo{};
        tile_info_map = tile_info.CreateUnorderedMap();
        continue;
      }
      std::string key = line.substr(0, delimiterPos);
      std::string value = line.substr(delimiterPos + 1);

      auto it = tile_info_map.find(key);
      if (it != tile_info_map.end()) {
        *(it->second) = value;
      }
    }
  }
  for (auto& tile : tiles_info_) {
    if (tile.pos_x_str.empty() || tile.pos_y_str.empty()) {
      throw std::runtime_error("wrong tile_pos data");
    }
    tile.pos_x = std::stoi(tile.pos_x_str);
    tile.pos_y = std::stoi(tile.pos_y_str);
  }
  // sort by pos starting from top-left corner
  std::sort(tiles_info_.begin(), tiles_info_.end(),
            [](const auto& tile_info_1, const auto& tile_info_2) {
    return tile_info_1.pos_x < tile_info_2.pos_x ||
                     (tile_info_1.pos_x == tile_info_2.pos_x &&
                      tile_info_1.pos_y < tile_info_2.pos_y);
  });
  if (tiles_info_.empty()) { // TODO; generate on our own
    throw std::runtime_error("no height map");
  }
}

void TileRenderer::Render() const {
  if (IsTerrainVisible()) {
    terrain_renderer_.Render();
  }
  if (IsWaterVisible()) {
    water_renderer_.Render();
  }
  if (IsRoadsVisible()) {
    roads_renderer_.Render();
  }
  if (IsFencesVisible()) {
    fences_renderer_.Render();
  }
  if (IsPlacementVisible()) {
    placement_renderer_.Render();
  }
  if (IsObjectsVisible()) {
    objects_renderer_.Render();
  }
}

void TileRenderer::RenderPickingAll() const {
  RenderPickingTerrain();
  RenderPickingWater();
  RenderPickingFences();
  RenderPickingObjects();
}

void TileRenderer::RenderPickingTerrain() const {
  if (IsTerrainVisible()) {
    terrain_renderer_.RenderPicking();
  }
}

void TileRenderer::RenderPickingWater() const {
  if (IsWaterVisible()) {
    water_renderer_.RenderPicking();
  }
}

void TileRenderer::RenderPickingFences() const {
  if (IsFencesVisible()) {
    fences_renderer_.RenderPicking();
  }
}

void TileRenderer::RenderPickingObjects() const {
  if (IsObjectsVisible()) {
    objects_renderer_.RenderPicking();
  }
}

void TileRenderer::DeInitMapScaleUbo() {
  glDeleteBuffers(GL_UNIFORM_BUFFER, &map_scale_ubo_);
}

void TileRenderer::UpdateScale() {
  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(map_scale_, map_scale_, map_scale_));
  glBindBuffer(GL_UNIFORM_BUFFER, map_scale_ubo_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(transform));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
  glGenBuffers(1, &map_scale_ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, map_scale_ubo_);
  // 64 for transform matrix (we modify only scale), float height map scale
  // and 3 other float for padding
  // (to ensure 16-byte alignment as per std140 layout rules)
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + 4 * sizeof(float),
               nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, details::kUboMapScaleBind, map_scale_ubo_);

  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(map_scale_, map_scale_, map_scale_));
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