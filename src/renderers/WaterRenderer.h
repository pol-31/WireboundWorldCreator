#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_

#include <memory>

#include "../core/Tile.h"
#include "Ocean.h"

class WaterRenderer {
 public:
  WaterRenderer(Tile& tile);

  ~WaterRenderer() { DeInit(); }

  void Render();

  void RenderRivers();

  void UpdateOcean(OceanTraits traits);

  void SetWaterColor(glm::vec4 color);

 private:
  void Init();

  void DeInit();

  Tile& tile_;
  GLuint vao_{0};
  Shader shader_;

  const int gGridSize = 64;
  std::array<GLuint, 64 * 64> patch_grid_;

  GLuint vbo_quad_{0};
  GLuint vbo_ids_{0};

  Texture tex_foam_;

  std::unique_ptr<Ocean> ocean_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
