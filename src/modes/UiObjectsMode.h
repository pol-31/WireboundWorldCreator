#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_

#include "../common/MouseTransform.h"
#include "../common/Vbos.h"
#include "../common/models/ModelManager.h"
#include "../core/UiCloning.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "IUiMode.h"
#include "UiEditObjects.h"

class UiObjectsMode final : public IUiMode {
 public:
  UiObjectsMode(UiSharedResources& ui_shared_resources, UiSlots& ui_slots,
                WindowQueue& window_queue, TextRenderer& text_renderer,
                ModelManager& mdl_manager);

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

  void UpdateModelsList();

  void SpawnObject(GLuint pressed_id);

  // todo;O_O names
  /// to transform selected objects
  void UpdateTranslateForSelected();
  void UpdateRotateForSelected();
  void UpdateScaleForSelected();

  UiCloning ui_cloning_;  // key C to create array/line-s of objects

  bool anything_selected_ = false;

  UiSelection ui_selection_;

  MouseTransform mouse_transform_;

  std::vector<BaseInstanceData> models_;  // TODO: mdl_manager/loader
  UiSlots& ui_slots_;
  UiEditObjects ui_edit_;

  ModelManager& mdl_manager_;

  glm::vec3 prev_translate_ = glm::vec3(0.0f);
  glm::quat prev_rotate_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 prev_scale_ = glm::vec3(1.0f);
  glm::vec3 cur_translate_ = glm::vec3(0.0f);
  glm::quat cur_rotate_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 cur_scale_ = glm::vec3(1.0f);

  //  UiEventHandler<
  //      static_cast<int>(data::VboIdMain::kObjectsHuman) -
  //      static_cast<int>(data::VboIdMain::kObjectsObjectsMode) + 1
  //      > ui_event_handler_;
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

/// cloning (C key)
void CursorPosCallback_C(GLFWwindow* window, double xpos, double ypos);

void MouseButtonCallback_C(GLFWwindow* window, int button, int action,
                           int mods);

}  // namespace objects

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_
