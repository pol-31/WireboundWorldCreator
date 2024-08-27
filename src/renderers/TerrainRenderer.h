#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_

#include <glm/glm.hpp>

#include "../core/Tile.h"
#include "../common/Paths.h"
#include "../common/Shader.h"

class TerrainRenderer {
 public:
  TerrainRenderer(Tile& tile, const Paths& paths);

  void Render() const;

  //TODO: fbo shoudl be bind at Interface::Draw() or somewhere else
  void RenderPicking() const;

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const;

 private:
  void Init();

  Tile& tile_;
  GLuint vao_{0};
  GLuint vbo_{0};
  Shader shader_;
  Shader shader_picking_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
