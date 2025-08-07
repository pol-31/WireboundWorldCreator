#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_

#include <glm/glm.hpp>

#include "../core/Tile.h"
#include "../common/Paths.h"
#include "../common/Shader.h"

class TerrainInstanceData;

class TerrainRenderer {
 public:
  TerrainRenderer(Tile& tile, const Paths& paths);

  ~TerrainRenderer() {
    DeInit();
  }

  void Render();

  //TODO: fbo shoudl be bind at Interface::Draw() or somewhere else
  void RenderPicking() const;

  void Render(TerrainInstanceData* terrain);

  void RenderPicking(TerrainInstanceData* terrain) const;

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const;

 private:
  void Init();

  void DeInit();

  const int gGridSize = 64;
  std::array<GLuint, 64 * 64> patch_grid_;

  Tile& tile_;
  GLuint vao_{0};
  GLuint vbo_quad_{0};
  GLuint vbo_ids_{0};
  Shader shader_;
  Shader shader_picking_;
  Texture nmap_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
