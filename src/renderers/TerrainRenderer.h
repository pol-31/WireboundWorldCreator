#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_

#include <glm/glm.hpp>

#include "../core/Tile.h"
#include "../common/Paths.h"
#include "../common/Shader.h"
#include "../common/Material.h"

struct BaseInstanceData;
struct TerrainInstanceData;

class TerrainRenderer {
 public:
  TerrainRenderer(Tile& tile, const Paths& paths);

  ~TerrainRenderer() {
    DeInit();
  }

  void Render();

  void RenderSubtract(const Texture& tex_subtract);

  void RenderPicking() const;

  void RenderSelection(
      const Texture32F* surface,
      const Texture& selection_mask, glm::vec3 color);

  void Render(TerrainInstanceData* terrain);

  void RenderWireframe(
      TerrainInstanceData* terrain, BaseInstanceData* data);

  void RenderPicking(TerrainInstanceData* terrain) const;

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const;

 void UpdateTransformUniform(glm::mat4 mat);

 private:
  static void LoadMaterialTexture(int layer, std::string_view path);

  void InitAlbedo();

  void InitNormal();

  void InitAo();

  void Init();

  void DeInit();

  const int gGridSize = 64;
  std::array<GLuint, 64 * 64> patch_grid_;

  Tile& tile_;
  GLuint vao_{0};
  GLuint vbo_quad_{0};
  GLuint vbo_ids_{0};
  Shader shader_;
  Shader shader_subtract_;
  Shader shader_selection_;
  Shader shader_wireframe_;
  Shader shader_picking_;
  Texture nmap_;

  GLuint border_vao_ = 0;
  GLuint border_vbo_ = 0;
  GLuint border_ebo_ = 0;
  Shader border_shader_;
  Texture border_tex_;

  Material mat_grass_;
  Material mat_mud_;
  Material mat_rock_;
  Material mat_sand_;

  TerrainMaterial material_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
