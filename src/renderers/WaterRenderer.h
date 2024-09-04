#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_

#include "../core/Tile.h"
#include "../common/Paths.h"

#include "water/WavesGenerator.h"

class WaterRenderer {
 public:
  WaterRenderer(Tile& tile, const Paths& paths);

  void Render();

  void RenderPicking() const;

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const;

 private:
  void Init();

  Tile& tile_;
  GLuint vao_{0};
  GLuint vbo_{0};
  Shader shader_;
  Shader shader_picking_;

  WavesGenerator waves_generator_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
