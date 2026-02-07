#ifndef WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string_view>
#include <vector>

#include "../common/GraphBakeConfig.h"
#include "../renderers/TerrainRenderer.h"
#include "../renderers/Vegetation.h"
#include "../renderers/WaterRenderer.h"
#include "Environment.h"
#include "Tile.h"

class Camera;

class TileRenderer {
 public:
  TileRenderer();

  void Render();

  void RenderInGame(const Camera* camera);

  void RenderUiTerrain(const Texture& tex_subtract);

  void RenderPicking();

  void UpdatePipeline();

  /// --- graphs (roads, river) ---
  /// MurMur3 0.0f-1.0f
  static float Hash(uint32_t x);

  void BakeGraph(const GraphBakeConfig& config,
                 const std::vector<float>& heights_in,
                 std::vector<float>& heights_out,
                 std::vector<uint8_t>& distances, glm::uvec2 a, glm::uvec2 b,
                 float height_a, float height_b);

  void BakeGraphs(const GraphBakeConfig& config,
                  const std::vector<float>& heights_in,
                  std::vector<float>& heights_out,
                  std::vector<uint8_t>& distances,
                  const std::vector<MapPoint>& points,
                  const std::vector<glm::uvec2>& joints);
  /// --- ---
  ///

  void BakeRivers(const std::vector<float>& heights_in);
  void BakeRoads(const std::vector<float>& heights_in);


void Build(const std::vector<float>& originalHeightMap);

float GetMaxHeight(int level, int x, int y) const;

glm::vec3 CastRay(glm::vec3 start, glm::vec3 dir, float maxDist);

  Tile cur_tile_;
  GeoClipmaps mesh_;

  Environment environment_;

  TerrainRenderer terrain;
  WaterRenderer water;
  Vegetation vegetation;

  Shader shader_apply_rivers_roads_;

  bool show_terrain_ = true;
  bool show_water_ = true;
  bool show_objects_ = true;
  bool show_placement_ = true;
  bool show_biomes_ = true;
  bool show_tiles_ = true;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_
