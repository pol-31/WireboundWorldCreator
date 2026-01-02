#ifndef WIREBOUNDWORLDCREATOR_SRC_TILE_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILE_H_

#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "../common/Texture.h"

/// contains all data for current tile
struct Tile {
  /// shared for tiles
  float map_scale = 64.0f;
  float target_map_scale = 64.0f;

  int pos_x = 0;
  int pos_y = 0;

  Texture32F map_terrain_height{};  // r32f

  Texture map_terrain_normal{};  // rg8
  Texture map_terrain_slope{};   // r8
  Texture map_terrain_ao{};      // r8
  Texture map_terrain_splat{};   // rgba8

  Texture32F map_terrain_erosion_thermal{};    // r32f
  Texture32F map_terrain_erosion_hydraulic{};  // r32f
  Texture32F map_water_accum{};                // r32f
  Texture map_water_flow{};                    // rg8

  Texture map_river_mask; // temp

  Texture map_water_height{};  // rg8

  std::vector<float> terrain_heights_;
  std::vector<uint8_t> water_heights_;

  /// we need this at the beginning of working with each tile, so we could
  /// remove all last point sets and thus restore water height map to its
  /// initial state (input map_water_height + water from external tiles)
  std::vector<uint8_t> water_heights_init_;

  Tile();

  void OnScroll(float yoffset);

  void UpdateMapScale(float delta_time);

  void UpScale();

  void DownScale();

  void UpdateTerrainHmap(Texture32F&& hmap);

  /// reset all baked hmaps (but not sources & objects with pos)
  void ResetTerrain();
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_TILE_H_
