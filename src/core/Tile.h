#ifndef WIREBOUNDWORLDCREATOR_SRC_TILE_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILE_H_

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "../common/ArbitraryGraph.h"
#include "../common/Texture.h"

//TODO: biome - at least one for each tile; we don't rely on neighbours,
// like at water, so need to set all biomes even on edges

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

  std::unordered_map<std::string, std::string*> CreateUnorderedMap();
};

// for roads and fences
struct GraphTraits {
  ArbitraryGraph graph;
  int type_id; //TODO; map id::type (roads, fences)

  static std::vector<GraphTraits> Parse(std::string_view path);
  static void Serialize(std::string_view path,
                        std::vector<GraphTraits> graphs);
};

//TODO: is this all needed data for objects & biomes

struct ObjectTraits {
  int model_id; //TODO: map id::path
  glm::vec3 position; //3rd coord for placing onto other objects
  glm::vec3 scale;
  glm::vec3 rotation;
  static std::vector<ObjectTraits> Parse(std::string_view path);
  static void Serialize(std::string_view path, std::vector<ObjectTraits> objects);
};

struct BiomeTraits {
  int biome_id; //TODO: map id::traits
  static std::vector<BiomeTraits> Parse(std::string_view path);
  static void Serialize(std::string_view path, std::vector<BiomeTraits> biomes);
};

// struct contains all data for current tile. The difference between Tile and
// TileInfo is that Tile creates OpenGL objects (Texture, Shader),
// while TileInfo keeps only "raw" data like paths, etc
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
  std::vector<GraphTraits> graph_water{};
  std::vector<GraphTraits> graph_roads{};
  std::vector<GraphTraits> graph_fences{};
  Texture map_placement_trees{};
  Texture map_placement_bushes{};
  Texture map_placement_tall_grass{};
  Texture map_placement_undergrowth{};
  std::vector<ObjectTraits> points_objects{};
  std::vector<BiomeTraits> points_biomes{}; // material effect

  //TODO; indeed, wisdom here is
  /// need to duplicate both on GPU and CPU:
  /// for gpu to render; for cpu to dynamically obtain object y pos
  std::vector<uint8_t> terrain_heights_;
  std::vector<uint8_t> water_heights_;

  Tile() = default;
  explicit Tile(const TileInfo& tile_info);
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_TILE_H_
