#ifndef WIREBOUNDWORLDCREATOR_SRC_TILE_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILE_H_

#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "../render/Texture.h"
#include "../modes/traits/BiomeTraits.h"
#include "../modes/traits/ObjectTraits.h"
#include "../modes/traits/OceanTraits.h"
#include "../modes/traits/RiverTraits.h"
#include "../modes/traits/RoadTraits.h"
#include "../modes/traits/TerrainTraits.h"

static std::vector<TerrainTraits> ReadTerrain(std::istream& in);
void WriteTerrain(std::ostream& out, const std::vector<TerrainTraits>& terrain);

static std::vector<BiomeTraits> ReadBiomes(std::istream& in);
void WriteBiomes(std::ostream& out, const std::vector<BiomeTraits>& biomes);

static std::vector<RoadTraits> ReadRoads(std::istream& in);
void WriteRoads(std::ostream& out, const std::vector<RoadTraits>& roads);

static std::vector<RiverTraits> ReadRivers(std::istream& in);
void WriteRivers(std::ostream& out, const std::vector<RiverTraits>& rivers);

static std::vector<OceanTraits> ReadOceans(std::istream& in);
void WriteOceans(std::ostream& out, const std::vector<OceanTraits>& oceans);

static std::vector<ObjectTraits> ReadObjects(std::istream& in);
void WriteObjects(std::ostream& out, const std::vector<ObjectTraits>& objects);

/// contains all data for current tile
struct Tile {
  /// shared for tiles
  float map_scale = 64.0f;
  float target_map_scale = 64.0f;

  int pos_x = 0;
  int pos_y = 0;

  /// --- in ---
  std::vector<TerrainTraits> terrain_data;
  std::vector<BiomeTraits> biomes_data;
  std::vector<RoadTraits> roads_data;
  std::vector<RiverTraits> rivers_data;
  std::vector<OceanTraits> ocean_data;
  std::vector<ObjectTraits> objects_data;

  Texture tex_placement_trees_;
  Texture tex_placement_bushes_;
  Texture tex_placement_tall_grass_;
  Texture tex_placement_undergrowth_;

  Texture32F map_ocean_surface_;

  /// --- out ---
  Texture32F map_terrain_height{};  // r32f
  Texture map_terrain_normal{};     // rg8
  Texture map_terrain_slope{};      // r8
  Texture map_terrain_ao{};         // r8
  Texture map_terrain_splat{};      // rgba8

  Texture32F map_terrain_erosion_thermal{};    // r32f
  Texture32F map_terrain_erosion_hydraulic{};  // r32f
  Texture32F map_water_accum{};                // r32f
  Texture map_water_flow{};                    // rg8

  // placement
  Texture tex_vegetation_mask;
  std::vector<GLuint> placement_trees_;
  std::vector<GLuint> placement_bushes_;
  std::vector<GLuint> placement_tall_grass_;
  std::vector<GLuint> placement_undergrowth_;

  // roads
  Texture32F tex_roads_deform_;
  Texture32F tex_roads_df_;
  Texture tex_roads_mask_;

  // rivers
  Texture32F tex_rivers_deform_;
  Texture32F tex_rivers_df_;
  Texture tex_rivers_mask_;

  Texture32F map_terrain_height_raw_;  // before UpdatePipeline() to revert
  Texture32F map_water_height{};

  std::vector<float> terrain_heights_;
  std::vector<float> water_heights_;

  std::vector<std::vector<float>> m_maxPyramid;

  Tile();

  void OnScroll(float yoffset);

  void UpdateMapScale(float delta_time);

  /// reset all baked hmaps (but not sources & objects with pos)
  void ResetTerrain();

  float GetPositionY(float fx, float fy);

  float GetPositionY32(float fx, float fy);

  void Parse(std::string_view path);

  void Serialize(std::string_view path);
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_TILE_H_
