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

  /// for placement we have pipeline, where some placement modes (like trees)
  /// have priority over other (like grass), so firstly we place trees and
  /// then grass placed where trees wasn't placed is their placement areas
  /// intersects.
  /// It used everytime we switch tiles or edit existing placement heightmaps
  void UpdatePipeline();

  void RenderDraw() const;

 private:
  void Init();
  void InitPlacementPipeline();

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
