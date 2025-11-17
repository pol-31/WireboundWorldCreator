#ifndef WIREBOUNDWORLDCREATOR_SRC_TILE_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILE_H_

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "../common/Texture.h"
#include "../common/Details.h"

struct TileInfo {
  // storing as string is convenient for CreateUnorderedMap() call,
  // but we also need to sort it by pos, so need integer type
  int pos_x{-1};
  int pos_y{-1};
  std::string pos_x_str{};
  std::string pos_y_str{};
  std::string map_terrain_height{};
  std::string map_erosion_wear{}; // material effect
  std::string map_erosion_flow{}; // material effect
  std::string map_erosion_deposition{}; // material effect
  std::string map_terrain_cavity{}; // ssao
  std::string map_terrain_occlusion{};
  std::string map_terrain_normal{};
  std::string map_terrain_wetness{};
  std::string map_water_height{};
  std::string map_water_flow{};
  std::string graph_water{};
  // mesh? // vorticity? shoreline? TODO: how? what?
  std::string graph_roads{};
  std::string graph_fences{};
  std::string map_placement_trees{};
  std::string map_placement_bushes{};
  std::string map_placement_tall_grass{};
  std::string map_placement_undergrowth{};
  std::string points_objects{};
  std::string points_biomes{}; // material effect

  std::unordered_map<std::string, std::string*> CreateUnorderedMap() {
    return {
        {"pos_x", &pos_x_str},
        {"pos_y", &pos_y_str},
        {"map_terrain_height", &map_terrain_height},
        {"map_erosion_wear", &map_erosion_wear},
        {"map_erosion_flow", &map_erosion_flow},
        {"map_erosion_deposition", &map_erosion_deposition},
        {"map_terrain_cavity", &map_terrain_cavity},
        {"map_terrain_occlusion", &map_terrain_occlusion},
        {"map_terrain_normal", &map_terrain_normal},
        {"map_terrain_wetness", &map_terrain_wetness},
        {"map_water_height", &map_water_height},
        {"map_water_flow", &map_water_flow},
        {"graph_water", &graph_water},
        {"graph_roads", &graph_roads},
        {"graph_fences", &graph_fences},
        {"map_placement_trees", &map_placement_trees},
        {"map_placement_bushes", &map_placement_bushes},
        {"map_placement_tall_grass", &map_placement_tall_grass},
        {"map_placement_undergrowth", &map_placement_undergrowth},
        {"points_objects", &points_objects},
        {"tile_info", &points_biomes}
    };
  }
};

/// contains all data for current tile
struct Tile {
  /// shared for tiles
  float map_scale = 64.0f;

  /// unique for each tile
  int pos_x;
  int pos_y;
  Texture32F map_terrain_height{}; // r32f

  Texture map_terrain_normal{}; // rg8
  Texture map_terrain_slope{}; // r8
  Texture map_terrain_ao{}; // r8
  Texture map_terrain_splat{}; // rgba8

  Texture32F map_terrain_erosion_thermal{}; // r32f
  Texture32F map_terrain_erosion_hydraulic{}; // r32f
  Texture32F map_water_accum{}; // r32f
  Texture map_water_flow{}; // rg8

  Texture map_water_height{}; // rg8
  // other data serializing called individually (e.g. graphs, placement)

  //TODO; indeed, wisdom here is
  /// need to duplicate both on GPU and CPU:
  /// for gpu to render; for cpu to dynamically obtain object y pos
  std::vector<float> terrain_heights_;
  std::vector<uint8_t> water_heights_;

  /// we need this at the beginning of working with each tile, so we could
  /// remove all last point sets and thus restore water height map to its initial
  /// state (input map_water_height + water from external tiles)
  std::vector<uint8_t> water_heights_init_;

  explicit Tile(const TileInfo& tile_info);

  void UpScale();

  void DownScale();

  void UpdateTerrainHmap(Texture32F&& hmap);

  /// reset all baked hmaps (but not sources & objects with pos)
  void ResetTerrain();

  /* Can we compute only 9... why not - at least for grass and water yes:
   * 1 2 3 4 4 4 ...
   * 2 2 3 4 4 4 ...
   * 3 3 3 4 4 4 ...
   * 4 4 4 4 4 4 ...
   * 4 4 4 4 4 4 ...
   * ...
   * So we cull 3/4 and leave 90degrees view
   * But can we use discs, not static squares... hm...
   * */

  //  enum class Lod {
  //    kLowest,
  //    kLow,
  //    kMedium,
  //    kHight
  //  };

  // At one time we can draw (total_lods * lod_per_tile)
  // std::array<Lod, >;

  // grass blades have pos related to player, not global, so to draw,
  // we can use another transform matrix, that is related to player

  //struct SubTile
  // * we don;t need idx, cuz they are equal to Tile::sub_tiles_ indices
  // * as well as their positions
};



#endif  // WIREBOUNDWORLDCREATOR_SRC_TILE_H_
