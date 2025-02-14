#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_

#include <memory>

#include "../core/Tile.h"
#include "../common/Paths.h"
#include "water/Ocean.h"
#include "../environment/Environment.h"
#include "../environment/Wind.h"

class WaterRenderer {
 public:
  WaterRenderer(Tile& tile, const Paths& paths);

  void Render();

  void RenderPicking() const;

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const;

  void UpdateOcean(OceanTraits traits);

 private:
  void Init();

  const Paths& paths_;
  Tile& tile_;
  GLuint vao_{0};
  GLuint vbo_{0};
  Shader shader_;
  Shader shader_picking_;

  Environment environment_;

  std::unique_ptr<Ocean> ocean_; // TODO: try to stack, not heap
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
