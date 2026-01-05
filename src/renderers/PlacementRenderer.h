#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_

#include "../common/Texture.h"
#include "Grass.h"

class PlacementRenderer {
 public:
  PlacementRenderer() = default;

  void Render();

  void RenderPicking();

  /// density: 1.0f - every pixel, 0.0f - never
  /// seed: model / instance / tile id
  std::vector<GLuint> UpdatePipeline(
    Texture& placement, float density, uint32_t seed);

  /// MurmurHash3 finalizer
  float PlacementHash(uint32_t x);

 private:
  /// UpdatePipeline here, get its data and collect points
  Texture placement_temp_;
  Grass grass_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_
