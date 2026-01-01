#include "Tile.h"

#include <iostream>

Tile::Tile(const TileInfo& tile_info)
    : map_scale(0.1f) {
  // TODO; use placeholders (full black / full white texture)

  // position data already valid
  // (we've thrown at TileRenderer::Init()) in case of missing
  pos_x = tile_info.pos_x;
  pos_y = tile_info.pos_y;
  /// --- --- --- no LOAD --- --- ---
//  map_terrain_height = Texture32F(tile_info.map_terrain_height, GL_R32F);

  map_terrain_height = Texture32F(1024, GL_R32F);

  map_terrain_normal = Texture(1024, 1024, GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  map_terrain_slope = Texture(1024, 1024, GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  map_terrain_ao = Texture(1024, 1024, GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  map_terrain_splat = Texture(1024, 1024, GL_RGBA8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  map_terrain_erosion_thermal = Texture32F(1024, GL_R32F);
  map_terrain_erosion_hydraulic = Texture32F(1024, GL_R32F);
  map_water_accum = Texture32F(1024, GL_R32F);
  map_water_flow = Texture(1024, 1024, GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  map_water_height = Texture(1024, 1024, GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  // duplicating data both on CPU & GPU (see Tile.h for more info)
  terrain_heights_.resize(1024 * 1024);
  map_terrain_height.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, terrain_heights_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  water_heights_init_.resize(1024 * 1024);
  map_water_height.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                water_heights_init_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  /// see explanation at header file (Tile.h)
  water_heights_ = water_heights_init_;
}

void Tile::UpScale() {
  float new_map_scale = map_scale * details::kMapScaleFactor;
  map_scale = std::min(new_map_scale, 100.0f);
  std::cout << map_scale << std::endl;
}

void Tile::DownScale() {
  float new_map_scale = map_scale / details::kMapScaleFactor;
  map_scale = std::max(new_map_scale, 0.05f);
}

void Tile::OnScroll(float yoffset) {
  if (yoffset > 0.0f)
    target_map_scale *= details::kMapScaleFactor;
  else
    target_map_scale /= details::kMapScaleFactor;

  target_map_scale = std::clamp(target_map_scale, 0.05f, 100.0f);
}

void Tile::UpdateMapScale(float delta_time) {
  float response = glm::mix(6.0f, 14.0f,
    glm::clamp(glm::log(map_scale) / glm::log(100.0f), 0.0f, 1.0f));
  // const float response = 12.0f; // feel parameter
  map_scale += (target_map_scale - map_scale) *
               (1.0f - std::exp(-response * delta_time));
}

void Tile::UpdateTerrainHmap(Texture32F&& hmap) {
  map_terrain_height = std::move(hmap);
  map_terrain_height.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, terrain_heights_.data());
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Tile::ResetTerrain() {
  GLubyte clear_r = 0;
  GLubyte clear_rg[2] = {0, 0};
  GLubyte clear_rgba[4] = {0, 0, 0, 0};
  float clear_r32f = 0.0f;

  glClearTexImage(map_terrain_slope.GetId(),
                  0, GL_RED, GL_UNSIGNED_BYTE, &clear_r);
  glClearTexImage(map_terrain_ao.GetId(),
                  0, GL_RED, GL_UNSIGNED_BYTE, &clear_r);
  glClearTexImage(map_terrain_splat.GetId(),
                  0, GL_RGBA, GL_UNSIGNED_BYTE, clear_rgba);
  glClearTexImage(map_terrain_normal.GetId(),
                  0, GL_RG, GL_UNSIGNED_BYTE, clear_rg);
  glClearTexImage(map_water_flow.GetId(),
                  0, GL_RG, GL_UNSIGNED_BYTE, clear_rg);
  glClearTexImage(map_water_height.GetId(),
                  0, GL_RG, GL_UNSIGNED_BYTE, clear_rg);

  glClearTexImage(map_terrain_height.GetId(),
                  0, GL_RED, GL_FLOAT, &clear_r32f);
  glClearTexImage(map_terrain_erosion_thermal.GetId(),
                  0, GL_RED, GL_FLOAT, &clear_r32f);
  glClearTexImage(map_terrain_erosion_hydraulic.GetId(),
                  0, GL_RED, GL_FLOAT, &clear_r32f);
  glClearTexImage(map_water_accum.GetId(),
                  0, GL_RED, GL_FLOAT, &clear_r32f);

  std::fill(terrain_heights_.begin(), terrain_heights_.end(), 0.0f);
  std::fill(water_heights_.begin(), water_heights_.end(), 0);
  std::fill(water_heights_init_.begin(), water_heights_init_.end(), 0);
}
