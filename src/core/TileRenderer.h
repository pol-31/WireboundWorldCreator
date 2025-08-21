#ifndef WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string_view>
#include <vector>

#include "Tile.h"
#include "../common/Paths.h"

#include "../renderers/AllRenderers.h"

class TileRenderer {
 public:
  explicit TileRenderer(const Paths& paths);

  ~TileRenderer();

//  void ResetScale();

//  void UpScale();

//  void DownScale();

  void Render();

  void RenderPicking();

  glm::vec3 GetPosition(int vertex_id);

  std::vector<TileInfo> map_;
  Tile cur_tile_;

  TerrainRenderer terrain;
  WaterRenderer water;
  FencesRenderer fences;
  RoadsRenderer roads;
  ObjectsRenderer objects;
  PlacementRenderer placement;

  bool show_terrain_ = true;
  bool show_water_ = true;
  bool show_fences_ = true;
  bool show_roads_ = true;
  bool show_objects_ = true;
  bool show_placement_ = true;

  /// affects other renderers only
  bool show_biomes_ = true;
  bool show_tiles_ = true;

 private:
  static std::vector<TileInfo> LoadMap(std::string_view world_map);

//  void InitMapScaleUbo();

//  void DeInitMapScaleUbo();

//  void UpdateScale();

//  GLuint map_scale_ubo_ = 0;
//  float map_scale_ = 1.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_
