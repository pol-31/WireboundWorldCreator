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

  void DeInit();

  void UpdateRatio();

  glm::vec2 GetBillboardTranslate(glm::mat4 mvp, int idx);

  glm::mat4 GetPointMvpMatrix();

  UiSharedResources& ui_shared_resources_;
  UiDynamicSprite sp_layer_;
  UiDynamicSprite sp_frame_;

  /// order: x_pos, x_neg, y_pox, y_neg, z_pos, z_neg
  std::array<UiDynamicSprite, 6> sp_points_;
  std::array<glm::vec4, 6> pos_points_;

  Texture layer_tex_;
  GLuint layer_fbo_ = 0;

  UiHierarchy hierarchy_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UILAYERWIREFRAME_H_
