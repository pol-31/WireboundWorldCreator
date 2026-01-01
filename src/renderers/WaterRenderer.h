#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_

#include <memory>

#include "../core/Tile.h"
#include "../common/Paths.h"
#include "water/Ocean.h"

class WaterRenderer {
 public:
  WaterRenderer(Tile& tile, const Paths& paths);

  ~WaterRenderer() {
    DeInit();
  }

  void Render();

  void RenderPicking() const;

  void UpdateOcean(OceanTraits traits);

  void SetWaterColor(glm::vec4 color);

 private:
  void Init();

  void DeInit();

  const Paths& paths_;
  Tile& tile_;
  GLuint vao_{0};
  Shader shader_;
  Shader shader_picking_;


  const int gGridSize = 64;
  std::array<GLuint, 64 * 64> patch_grid_;

  GLuint vbo_quad_{0};
  GLuint vbo_ids_{0};

  Texture tex_foam_;

  std::unique_ptr<Ocean> ocean_; // TODO: try to stack, not heap
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
