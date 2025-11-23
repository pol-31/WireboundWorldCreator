#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_

#include "IUiMode.h"
#include "../common/Vbos.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "../common/MouseTransform.h"
#include "../common/models/ModelManager.h"

#include "UiTerrainWIndows.h" //todo; temp use UiObjectsWindows

class UiObjectsMode final : public IUiMode {
 public:
  UiObjectsMode(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer,
      Tile& cur_tile,
      ModelManager& mdl_manager);

  void Render() override;

  void RenderPicking() override;

  void Setup() override;

  int GetPrerenderTextIdStart() const noexcept override;

  int GetPrerenderTextIdEnd() const noexcept override;

  void HandleSelection();

  void CancelTransform();

  void ApplyTransform();

  void ResetTransform();

  void UpdateModelsList();

  void SpawnObject(GLuint pressed_id);

  // O_O
  void UpdateTranslateForSelected();
  void UpdateRotateForSelected();
  void UpdateScaleForSelected();

  bool anything_selected_ = false;

  UiSelection ui_selection_;

  MouseTransform mouse_transform_;

  UiSlots ui_slots_;
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

void BindCallbacksDefault();

void ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

//-


void CursorPosCallback_Mmb(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_MmbShift(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos);

/// transform

void BindCallbacksTransform();

void MouseButtonCallbackTransform(
    GLFWwindow* window, int button, int action, int mods);

void KeyCallbackTransform(
    GLFWwindow* window, int key, int scancode, int action, int mods);

void CursorPosCallback_G(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_R(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_S(
    GLFWwindow* window, double xpos, double ypos);

/// blocking

void MouseButtonCallback_Lmb(
    GLFWwindow* window, int button, int action, int mods);
void MouseButtonCallback_Mmb_MmbShift(
    GLFWwindow* window, int button, int action, int mods);
void KeyCallback_Blocked(
    GLFWwindow* window, int key, int scancode, int action, int mods);

} // namespace objects

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_
