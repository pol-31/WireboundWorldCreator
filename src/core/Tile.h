#ifndef WIREBOUNDWORLDCREATOR_SRC_TILE_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILE_H_

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "../common/Texture.h"

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
  int pos_x;
  int pos_y;
  Texture map_terrain_height{};
  Texture map_erosion_wear{}; // material effect
  Texture map_erosion_flow{}; // material effect
  Texture map_erosion_deposition{}; // material effect
  Texture map_terrain_cavity{}; // ssao
  Texture map_terrain_occlusion{};
  Texture map_terrain_normal{};
  Texture map_terrain_wetness{};
  Texture map_water_height{};
  Texture map_water_flow{};
  // other data serializing called individually (e.g. graphs, placement)

  //TODO; indeed, wisdom here is
  /// need to duplicate both on GPU and CPU:
  /// for gpu to render; for cpu to dynamically obtain object y pos
  std::vector<uint8_t> terrain_heights_;
  std::vector<uint8_t> water_heights_;

  /// we need this at the beginning of working with each tile, so we could
  /// remove all last point sets and thus restore water height map to its initial
  /// state (input map_water_height + water from external tiles)
  std::vector<uint8_t> water_heights_init_;

  explicit Tile(const TileInfo& tile_info) {
    // TODO; use placeholders (full black / full white texture)

    // position data already valid
    // (we've thrown at TileRenderer::Init()) in case of missing
    pos_x = tile_info.pos_x;
    pos_y = tile_info.pos_y;
    // map_terrain_height is necessary (if float GL_RED is ignored)
    map_terrain_height = Texture(tile_info.map_terrain_height, GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
    map_erosion_wear = Texture(tile_info.map_erosion_wear, GL_RED);
    map_erosion_flow = Texture(tile_info.map_erosion_flow, GL_RED);
    map_erosion_deposition = Texture(tile_info.map_erosion_deposition, GL_RGBA);
    map_terrain_cavity = Texture(tile_info.map_terrain_cavity, GL_RED);
    map_terrain_occlusion = Texture(tile_info.map_terrain_occlusion, GL_RED);
    //  map_terrain_normal = Texture(tile_info.map_terrain_normal, GL_RED);
    map_terrain_wetness = Texture(tile_info.map_terrain_wetness, GL_RED);

    if (tile_info.map_water_height.empty()) {
      map_water_height = Texture(1024, 1024, GL_RED, GL_LINEAR, GL_CLAMP_TO_EDGE);
    } else {
      map_water_height = Texture(tile_info.map_water_height, GL_RED);
    }
    //  InitHeightMap(tile_info.map_water_height, map_water_height);

    map_water_flow = Texture(tile_info.map_water_flow, GL_RED);

    // duplicating data both on CPU & GPU (see Tile.h for more info)
    terrain_heights_.resize(1024 * 1024);
    map_terrain_height.Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                  terrain_heights_.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    water_heights_init_.resize(1024 * 1024);
    map_water_height.Bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                  water_heights_init_.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    /// see explanation at header file (Tile.h)
    water_heights_ = water_heights_init_;
  }

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
