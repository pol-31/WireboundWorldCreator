#ifndef WIREBOUNDWORLDCREATOR_SRC_TILE_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILE_H_

#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "../common/GraphBakeConfig.h"
#include "../common/MapPoint.h"
#include "../common/Texture.h"

class RiverTraits;
class RoadTraits;
class ObjGraphTraits;

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

  Texture map_river_mask;  // temp

  /// placement
  Texture tex_placement_trees_;
  Texture tex_placement_bushes_;
  Texture tex_placement_tall_grass_;
  Texture tex_placement_undergrowth_;
  Texture tex_vegetation_mask;

  std::vector<GLuint> placement_trees_;
  std::vector<GLuint> placement_bushes_;
  std::vector<GLuint> placement_tall_grass_;
  std::vector<GLuint> placement_undergrowth_;
  // TODO:
  //  - density constants: 0.001, 0.01, 0.1, 0.2
  //  - seeds: 0 1 2 3

  /// roads
  Texture32F tex_roads_deform_;
  Texture32F tex_roads_df_;
  Texture tex_roads_mask_;
  std::vector<RoadTraits>* roads_ = nullptr;

  /// rivers
  Texture32F tex_rivers_deform_;
  Texture32F tex_rivers_df_;
  Texture tex_rivers_mask_;
  std::vector<RiverTraits>* rivers_ = nullptr;

  Texture32F map_terrain_height_raw_;  // before UpdatePipeline() to revert
  Texture map_water_height{};          // rg8

  Texture32F map_ocean_surface_;

  std::vector<float> terrain_heights_;
  std::vector<uint8_t> water_heights_;

  /// we need this at the beginning of working with each tile, so we could
  /// remove all last point sets and thus restore water height map to its
  /// initial state (input map_water_height + water from external tiles)
  std::vector<uint8_t> water_heights_init_;

  std::vector<ObjGraphTraits>* objects_ = nullptr;

  Tile();

  void OnScroll(float yoffset);

  void UpdateMapScale(float delta_time);

  void UpScale();

  void DownScale();

  /// reset all baked hmaps (but not sources & objects with pos)
  void ResetTerrain();

  float GetPositionY(float fx, float fy);
  float GetPositionY32(float fx, float fy);
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_TILE_H_
