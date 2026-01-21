#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_

#include "../common/MapPoints.h"
#include "../common/MouseTransform.h"
#include "../common/models/ModelManager.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "IUiMode.h"
#include "UiEditObjects.h"

class UiObjectsMode final : public IUiMode {
 public:
  UiObjectsMode(UiSharedResources& ui_shared_resources, UiSlots& ui_slots,
                WindowQueue& window_queue, TextRenderer& text_renderer,
                UiEditSlots& ui_edit_slots, UiEditConfigSlTxt& value_config,
                ModelManager& mdl_manager);

  void Render() override;

  void RenderPicking() override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  void HandleSelection(const std::set<GLuint>& selected_ids);

  void CancelTransform();

  void ApplyTransform();

  void UpdateModelsList();

  UiSlots& ui_slots_;
  UiEditObjects ui_edit_;

  UiSelection ui_selection_;
  bool anything_selected_ = false;

  MouseTransform mouse_transform_;

  MapObjects map_points_;      // for graphs (e.g. fences)
  ModelManager& mdl_manager_;  // for separate objects (rotated / scaled)
};

namespace objects {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

/// selection
void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods);

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos);

/// transform
void BindCallbacksTransform();

void MouseButtonCallbackTransform(GLFWwindow* window, int button, int action,
                                  int mods);

void KeyCallbackTransform(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

void CursorPosCallback_G(GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_R(GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_S(GLFWwindow* window, double xpos, double ypos);

}  // namespace objects

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_
