#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UILAYERWIREFRAME_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UILAYERWIREFRAME_H_

#include <glad/glad.h>

#include "../common/Texture.h"
#include "../modes/UiSharedResources.h"
#include "Ui.h"
#include "UiComplex.h"

struct BaseInstanceData;
struct TerrainTraits;

class UiLayerWireframe {
 public:
  UiLayerWireframe(UiSharedResources& ui_shared_resources);

  UiLayerWireframe(UiLayerWireframe&& other) = delete;
  UiLayerWireframe(const UiLayerWireframe& other) = delete;

  UiLayerWireframe& operator=(UiLayerWireframe&& other) = delete;
  UiLayerWireframe& operator=(const UiLayerWireframe& other) = delete;

  ~UiLayerWireframe() { DeInit(); }

  void RenderLayerWireframe(TerrainTraits* terrain);

  void RenderPickingLayerWireframe();

  void UpdateLayerWireframe(TerrainTraits* terrain);

 private:
  void Init();

  void InitFbo();

  void DeInit();

  void UpdateRatio();

  void RenderAxis(int first_idx);

  glm::vec3 GetBillboardTranslate(glm::mat4 mvp, int idx);

  glm::mat4 GetPointMvpMatrix();

  UiSharedResources& ui_shared_resources_;
  UiSprite sp_layer_;
  UiSprite sp_frame_;

  /// order: x_pos, x_neg, y_pox, y_neg, z_pos, z_neg
  std::array<UiSprite, 6> sp_points_;
  std::array<glm::vec4, 6> pos_points_;

  Texture layer_tex_;
  GLuint layer_fbo_ = 0;

  UiHierarchy hierarchy_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UILAYERWIREFRAME_H_
