#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_

#include <glm/glm.hpp>

#include "../common/Material.h"
#include "../common/Shader.h"
#include "../core/Tile.h"
#include "GeoClipmaps.h"

struct BaseInstanceData;
struct TerrainTraits;

class TerrainRenderer {
 public:
  TerrainRenderer(Tile& tile, GeoClipmaps& mesh);

  void Render();

  void RenderInGame();

  void RenderSubtract(const Texture& tex_subtract);

  void RenderPicking() const;

  void RenderSelection(const Texture32F* surface, const Texture& selection_mask,
                       glm::vec3 color);

  void RenderWireframe(TerrainTraits* terrain);

 private:
  static void LoadMaterialTexture(int layer, std::string_view path);

  void InitMaterial();

  void InitAlbedo();

  void InitNormal();

  void InitAo();

  void UpdateShaders();  // for shader dbg in run-time

  void BindUniforms();

  Tile& tile_;
  Shader shader_;
  Shader shader_game_;
  Shader shader_subtract_;
  Shader shader_selection_;
  Shader shader_wireframe_;
  Shader shader_picking_;
  Texture nmap_;

  Material mat_grass_;
  Material mat_mud_;
  Material mat_rock_;
  Material mat_sand_;

  TerrainMaterial material_;
  GeoClipmaps& mesh_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_TERRAINRENDERER_H_
