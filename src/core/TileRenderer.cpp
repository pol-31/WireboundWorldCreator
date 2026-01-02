#include "TileRenderer.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "../common/Details.h"
#include "../common/OpenGlUtility.h"

TileRenderer::TileRenderer()
    : terrain(cur_tile_),
      water(cur_tile_) {}

void TileRenderer::Render() {
  environment_.Update();
  if (show_terrain_) {
    terrain.Render();
  }
  if (show_water_) {
    water.Render();
  }
  if (show_placement_) {
    placement.Render();
  }
}

void TileRenderer::RenderUiTerrain(const Texture& tex_subtract) {
  environment_.Update();
  if (show_terrain_) {
    terrain.RenderSubtract(tex_subtract);
  }
  if (show_water_) {
    water.Render();
  }
  if (show_placement_) {
    placement.Render();
  }
}

void TileRenderer::RenderPicking() {
  if (show_terrain_) {
    terrain.RenderPicking();
  }
  if (show_placement_) {
    placement.RenderPicking();
  }
}
