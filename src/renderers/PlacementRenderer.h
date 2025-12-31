#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_

#include "../core/Tile.h"
#include "../common/Paths.h"
#include "../common/Texture.h"

#include "placement/AllPlacement.h"

class PlacementRenderer {
 public:
  PlacementRenderer(Tile& tile, const Paths& paths);

  void Render();

  void RenderPicking() const {}

  std::vector<GLuint> UpdatePipeline(
    Texture& placement, int density_level = 0);

  void RenderDraw() const;

 private:
  void Init();

  Tile& tile_;
  GLuint vao_{0};
  GLuint vbo_{0};
  Shader shader_;
  Shader poisson_shader_;

  Texture density_low_;
  Texture density_medium_low_;
  Texture density_medium_;
  Texture density_medium_high_;
  Texture density_high_;
  Texture density_very_high_;
  Texture density_ultra_high_;
  Texture density_extreme_;

  /// UpdatePipeline here, get its data and collect points
  Texture placement_temp_;

  Grass grass_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_
