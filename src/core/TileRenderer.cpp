#include "TileRenderer.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>

#include <glm/gtc/type_ptr.hpp>

#include "../common/Details.h"
#include "../common/ShadersBinding.h"
#include "../common/OpenGlUtility.h"

TileRenderer::TileRenderer(const Paths& paths)
    : map_(LoadMap(paths.world_map)),
      cur_tile_(map_[0]),
      environment_(paths),
      terrain(cur_tile_, paths),
      water(cur_tile_, paths),
      fences(cur_tile_, paths),
      roads(cur_tile_, paths),
      objects(cur_tile_, paths),
      placement(cur_tile_, paths) {
//  InitMapScaleUbo();
}

TileRenderer::~TileRenderer() {
//  DeInitMapScaleUbo();
}

/*void TileRenderer::ResetScale() {
  map_scale_ = 1.0f;
  UpdateScale();
}

void TileRenderer::UpScale() {
  map_scale_ *= details::kMapScaleFactor;
  UpdateScale();
}

void TileRenderer::DownScale() {
  map_scale_ /= details::kMapScaleFactor;
  UpdateScale();
}*/

void TileRenderer::Render() {
  environment_.Update();
  if (show_terrain_) {
    terrain.Render();
  }
  if (show_water_) {
    water.Render();
  }
  if (show_fences_) {
    fences.Render();
  }
  if (show_roads_) {
    roads.Render();
  }
  if (show_objects_) {
    objects.Render();
  }
  if (show_placement_) {
    placement.Render();
  }
}

void TileRenderer::RenderUiTerrain(const Texture& tex_subtract) {
  environment_.Update();
  if (show_terrain_) {
    terrain.RenderSubtract(tex_subtract);
  }
  if (show_water_) {
    water.Render();
  }
  if (show_fences_) {
    fences.Render();
  }
  if (show_roads_) {
    roads.Render();
  }
  if (show_objects_) {
    objects.Render();
  }
  if (show_placement_) {
    placement.Render();
  }
}

void TileRenderer::RenderPicking() {
  if (show_terrain_) {
    terrain.RenderPicking();
  }
  if (show_water_) {
    water.RenderPicking();
  }
  if (show_fences_) {
    fences.RenderPicking();
  }
  if (show_roads_) {
    roads.RenderPicking();
  }
  if (show_objects_) {
    objects.RenderPicking();
  }
  if (show_placement_) {
    placement.RenderPicking();
  }
}

glm::vec3 TileRenderer::GetPosition(int vertex_id) {
  return glm::vec3(0.0f);
  // if (vertex_id < 1024 * 1024) {
  //   return terrain.GetYPosition(vertex_id);
  // } else if (vertex_id < 2 * 1024 * 1024) {
  //   return water.GetYPosition(vertex_id);
  // } else if (vertex_id < 3 * 1024 * 1024) {
  //   return fences.GetYPosition(vertex_id);
  // } else {
  //   return objects.GetYPosition(vertex_id);
  // }
}

std::vector<TileInfo> TileRenderer::LoadMap(std::string_view world_map) {
  std::ifstream file(world_map.data());
  if (!file.is_open()) {
    throw std::runtime_error("Error opening map file");
  }
  std::vector<TileInfo> map;
  TileInfo tile_info;
  auto tile_info_map = tile_info.CreateUnorderedMap();

  std::string line;
  while (std::getline(file, line)) {
    size_t delimiterPos = line.find('=');
    if (delimiterPos != std::string::npos) {
      // "=" means end of current tile
      if (delimiterPos == 0) {
        map.push_back(tile_info);
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
  for (auto& tile : map) {
    if (tile.pos_x_str.empty() || tile.pos_y_str.empty()) {
      throw std::runtime_error("wrong tile_pos data");
    }
    tile.pos_x = std::stoi(tile.pos_x_str);
    tile.pos_y = std::stoi(tile.pos_y_str);
  }
  // sort by pos starting from top-left corner
  std::sort(map.begin(), map.end(),
            [](const auto& tile_info_1, const auto& tile_info_2) {
              return tile_info_1.pos_x < tile_info_2.pos_x ||
                     (tile_info_1.pos_x == tile_info_2.pos_x &&
                      tile_info_1.pos_y < tile_info_2.pos_y);
            });
  if (map.empty()) { // TODO; generate on our own
    throw std::runtime_error("no height map");
  }
  return map;
}

/*
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

void TileRenderer::DeInitMapScaleUbo() {
  glDeleteBuffers(1, &map_scale_ubo_);
}

void TileRenderer::UpdateScale() {
  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(map_scale_, map_scale_, map_scale_));
  utility::UpdateUbo(map_scale_ubo_, 0, sizeof(glm::mat4),
                     glm::value_ptr(transform));
}*/
