#ifndef WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string_view>
#include <vector>

#include "../renderers/PlacementRenderer.h"
#include "../renderers/TerrainRenderer.h"
#include "../renderers/WaterRenderer.h"
#include "Environment.h"
#include "Tile.h"

class TileRenderer {
 public:
  TileRenderer();

  void Render();

  void RenderUiTerrain(const Texture& tex_subtract);

  void RenderPicking();

  Tile cur_tile_;

  Environment environment_;

  TerrainRenderer terrain;
  WaterRenderer water;
  PlacementRenderer placement;

  bool show_terrain_ = true;
  bool show_water_ = true;
  bool show_objects_ = true;
  bool show_placement_ = true;
  bool show_biomes_ = true;
  bool show_tiles_ = true;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_TILERENDERER_H_
