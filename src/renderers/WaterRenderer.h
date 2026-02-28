#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_

#include <memory>

#include "../core/Tile.h"
#include "GeoClipmaps.h"
#include "Ocean.h"

class WaterRenderer {
 public:
  WaterRenderer(Tile& tile, GeoClipmaps& mesh);

  void Render();

  void RenderInGame();

  void RenderRivers();

  void UpdateOcean(OceanTraits traits);

  void SetWaterColor(glm::vec4 color);

 private:
  void BindUniforms();

  Tile& tile_;
  Shader shader_;
  Shader shader_game_;

  Texture tex_foam_;
  std::unique_ptr<Ocean> ocean_;
  GeoClipmaps& mesh_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
