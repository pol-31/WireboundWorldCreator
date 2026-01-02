#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_

#include "../common/Texture.h"
#include "../core/Tile.h"
#include "Grass.h"

class PlacementRenderer {
 public:
  PlacementRenderer();

  void Render();

  void RenderPicking();

  std::vector<GLuint> UpdatePipeline(Texture& placement, int density_level = 0);

 private:
  void Init();

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
