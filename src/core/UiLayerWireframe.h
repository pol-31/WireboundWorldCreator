#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UILAYERWIREFRAME_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UILAYERWIREFRAME_H_

#include <glad/glad.h>

#include "Ui.h"
#include "UiComplex.h"
#include "../modes/UiSharedResources.h"
#include "../common/Texture.h"

struct TerrainInstanceData;

class UiLayerWireframe {
 public:
  UiLayerWireframe(UiSharedResources& ui_shared_resources);

  UiLayerWireframe(UiLayerWireframe&& other) noexcept;
  UiLayerWireframe(const UiLayerWireframe& other) = delete;

  UiLayerWireframe& operator=(UiLayerWireframe&& other) = delete;
  UiLayerWireframe& operator=(const UiLayerWireframe& other) = delete;

  ~UiLayerWireframe() {
    DeInit();
  }

  void RenderLayerWireframe(TerrainInstanceData* terrain);

  void RenderPickingLayerWireframe();

  void UpdateLayerWireframe(TerrainInstanceData* terrain);

 private:
  void Init();

  void InitFbo();

  void InitBillboards();

  void DeInit();

  void UpdateRatio();

  glm::mat4 GetBillboardMatrix(
      const glm::vec3& position, const glm::mat4& view,
      const glm::mat4& projection);

  UiSharedResources& ui_shared_resources_;
  UiDynamicSprite layer_;
  UiDynamicSprite frame_;
  Texture layer_tex_;
  GLuint layer_fbo_ = 0;

  GLuint billboard_vao_ = 0;
  GLuint billboard_vbo_ = 0;
  Shader shader_billboard_;
  // don't need UiDynamicSprite for billboard +-XYZ, keep tex_coords only

  UiHierarchy hierarchy_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UILAYERWIREFRAME_H_
