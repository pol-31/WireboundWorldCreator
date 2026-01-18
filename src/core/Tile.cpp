#include "Tile.h"

#include <iostream>

#include "../common/Details.h"

Tile::Tile() : map_scale(0.1f) {
  float size = details::gTerrainSize;

  map_terrain_height = Texture32F(size, GL_R32F);

  map_terrain_normal = Texture(size, size, GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  map_terrain_slope = Texture(size, size, GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  glClearTexImage(map_terrain_slope.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                  nullptr);
  map_terrain_ao = Texture(size, size, GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  map_terrain_splat =
      Texture(size, size, GL_RGBA8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  map_river_mask = Texture(size, size, GL_R8);
  glClearTexImage(map_river_mask.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

  map_terrain_erosion_thermal = Texture32F(size, GL_R32F);
  map_terrain_erosion_hydraulic = Texture32F(size, GL_R32F);
  map_water_accum = Texture32F(size, GL_R32F);
  map_water_flow = Texture(size, size, GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  map_water_height = Texture(size, size, GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  // duplicating data both on CPU & GPU (see Tile.h for more info)
  terrain_heights_.resize(size * size);
  map_terrain_height.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, terrain_heights_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  water_heights_init_.resize(size * size);
  map_water_height.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                water_heights_init_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  map_ocean_surface_ = Texture32F(size, GL_R32F);
  glClearTexImage(map_ocean_surface_.GetId(), 0, GL_RED, GL_FLOAT, nullptr);

  tex_roads_deform_ = Texture32F(size, GL_R32F);
  glClearTexImage(tex_roads_deform_.GetId(), 0, GL_RED, GL_FLOAT, nullptr);
  tex_roads_df_ = Texture32F(size, GL_R32F);
  glClearTexImage(tex_roads_df_.GetId(), 0, GL_RED, GL_FLOAT, nullptr);
  tex_roads_mask_ = Texture(size, size, GL_R8);
  glClearTexImage(tex_roads_mask_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                  nullptr);

  tex_rivers_deform_ = Texture32F(size, GL_R32F);
  glClearTexImage(tex_rivers_deform_.GetId(), 0, GL_RED, GL_FLOAT, nullptr);
  tex_rivers_df_ = Texture32F(size, GL_R32F);
  glClearTexImage(tex_rivers_df_.GetId(), 0, GL_RED, GL_FLOAT, nullptr);
  tex_rivers_mask_ = Texture(size, size, GL_R8);

  tex_placement_trees_ =
      Texture(details::gTerrainSize, details::gTerrainSize, GL_R8);
  glClearTexImage(tex_placement_trees_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                  nullptr);
  tex_placement_bushes_ =
      Texture(details::gTerrainSize, details::gTerrainSize, GL_R8);
  glClearTexImage(tex_placement_bushes_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                  nullptr);
  tex_placement_tall_grass_ =
      Texture(details::gTerrainSize, details::gTerrainSize, GL_R8);
  glClearTexImage(tex_placement_tall_grass_.GetId(), 0, GL_RED,
                  GL_UNSIGNED_BYTE, nullptr);
  tex_placement_undergrowth_ =
      Texture(details::gTerrainSize, details::gTerrainSize, GL_R8);
  glClearTexImage(tex_placement_undergrowth_.GetId(), 0, GL_RED,
                  GL_UNSIGNED_BYTE, nullptr);
  tex_vegetation_mask = Texture(size, size, GL_R8);
  glClearTexImage(tex_vegetation_mask.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                  nullptr);

  map_terrain_height_raw_ = Texture32F(details::gTerrainSize, GL_R32F);
  glClearTexImage(map_terrain_height_raw_.GetId(), 0, GL_RED, GL_FLOAT,
                  nullptr);

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
  float response =
      glm::mix(6.0f, 14.0f,
               glm::clamp(glm::log(map_scale) / glm::log(100.0f), 0.0f, 1.0f));
  // const float response = 12.0f; // feel parameter
  map_scale += (target_map_scale - map_scale) *
               (1.0f - std::exp(-response * delta_time));
}

void Tile::ResetTerrain() {
  GLubyte clear_r = 0;
  GLubyte clear_rg[2] = {0, 0};
  GLubyte clear_rgba[4] = {0, 0, 0, 0};
  float clear_r32f = 0.0f;

  glClearTexImage(map_terrain_slope.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                  &clear_r);
  glClearTexImage(map_terrain_ao.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                  &clear_r);
  glClearTexImage(map_terrain_splat.GetId(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                  clear_rgba);
  glClearTexImage(map_terrain_normal.GetId(), 0, GL_RG, GL_UNSIGNED_BYTE,
                  clear_rg);
  glClearTexImage(map_water_flow.GetId(), 0, GL_RG, GL_UNSIGNED_BYTE, clear_rg);
  glClearTexImage(map_water_height.GetId(), 0, GL_RG, GL_UNSIGNED_BYTE,
                  clear_rg);

  glClearTexImage(map_terrain_height.GetId(), 0, GL_RED, GL_FLOAT, &clear_r32f);
  glClearTexImage(map_terrain_erosion_thermal.GetId(), 0, GL_RED, GL_FLOAT,
                  &clear_r32f);
  glClearTexImage(map_terrain_erosion_hydraulic.GetId(), 0, GL_RED, GL_FLOAT,
                  &clear_r32f);
  glClearTexImage(map_water_accum.GetId(), 0, GL_RED, GL_FLOAT, &clear_r32f);

  std::fill(terrain_heights_.begin(), terrain_heights_.end(), 0.0f);
  std::fill(water_heights_.begin(), water_heights_.end(), 0);
  std::fill(water_heights_init_.begin(), water_heights_init_.end(), 0);
}

float Tile::GetPositionY(float fx, float fy) {
  int x = std::min(1022, static_cast<int>(floorf(fx)));
  int y = std::min(1022, static_cast<int>(floorf(fy)));

  float tx = fx - x;  // 0..1
  float ty = fy - y;  // 0..1

  const auto& h = terrain_heights_;
  int stride = 1024;

  // sample 4 neighbors
  float h00 = h[y * stride + x];
  float h10 = h[y * stride + x + 1];
  float h01 = h[(y + 1) * stride + x];
  float h11 = h[(y + 1) * stride + x + 1];

  // bilinear interpolation
  float hx0 = h00 + (h10 - h00) * tx;
  float hx1 = h01 + (h11 - h01) * tx;
  return hx0 + (hx1 - hx0) * ty;
}

float Tile::GetPositionY32(float fx, float fy) {
  return GetPositionY(fx * 16.0f + 512.0f, fy * 16.0f + 512.0f);
}
