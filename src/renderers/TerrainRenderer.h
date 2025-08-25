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

  void RenderSelection(TerrainInstanceData* terrain,
                       const Texture& selection_mask, glm::vec3 color);

  void Render(TerrainInstanceData* terrain);

  void RenderWireframe(TerrainInstanceData* terrain, glm::vec3 color);

  void RenderPicking(TerrainInstanceData* terrain) const;

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const;

 void UpdateTransformUniform(glm::mat4 mat);

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
  Shader shader_selection_;
  Shader shader_wireframe_;
  Shader shader_picking_;
  Texture nmap_;

  GLuint border_vao_ = 0;
  GLuint border_vbo_ = 0;
  Shader border_shader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
