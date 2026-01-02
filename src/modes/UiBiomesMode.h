#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIBIOMESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIBIOMESMODE_H_

#include "../common/MouseTransform.h"
#include "../common/Vbos.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "IUiMode.h"
#include "UiEditBiomes.h"

class UiBiomesMode final : public IUiMode {
 public:
  UiBiomesMode(UiSharedResources& ui_shared_resources, UiSlots& ui_slots,
               WindowQueue& window_queue, TextRenderer& text_renderer);

  void Render() override;

  void RenderPicking() override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  int GetPrerenderTextIdStart() const noexcept override;

  int GetPrerenderTextIdEnd() const noexcept override;

  void RenderWorld() override;

  void RenderPickingWorld() override;

  void HandleSelection();

  void CancelTransform();

  void ApplyTransform();

  void ResetTransform();

  void SpawnObject(GLuint pressed_id);

  void UpdateTranslateForSelected();

  UiDynamicSprite sp_biome_;

  std::vector<BaseInstanceData> biomes_;
  UiSlots& ui_slots_;
  UiEditBiomes ui_edit_;
  UiSelection ui_selection_;
  MouseTransform mouse_transform_;

  bool anything_selected_ = false;
  glm::vec3 prev_translate_ = glm::vec3(0.0f);
  glm::vec3 cur_translate_ = glm::vec3(0.0f);
};

namespace biomes {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

/// selection
void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods);

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos);

/// transform (no rotation and scale - only translation)
void BindCallbacksTransform();

void MouseButtonCallbackTransform(GLFWwindow* window, int button, int action,
                                  int mods);

void KeyCallbackTransform(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

void CursorPosCallback_G(GLFWwindow* window, double xpos, double ypos);

}  // namespace biomes

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIBIOMESMODE_H_
