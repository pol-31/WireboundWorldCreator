#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "IUiMode.h"
#include "../common/Vbos.h"

#include "../core/Tile.h"
#include "../core/UiSlots.h"
#include "../core/UiComplex.h"

/// everything's public, otherwise need to make too much callback friends
class UiTerrainMode final : public IUiMode {
 public:
  explicit UiTerrainMode(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer,
      Tile& cur_tile);

  void Render() override;

  void RenderPicking() override;

  void BindCallbacks() override;

  int GetPrerenderTextIdStart() const noexcept override;

  int GetPrerenderTextIdEnd() const noexcept override;

  UiStaticSprite btn_update_;
  UiStaticSprite sprite_flatten_;
  UiToggle4 toggle_flatten_;

  UiSliderV3 slider_size_;
  UiSliderV3 slider_falloff_;

  UiStaticSprite btn_bake_;
  UiTerrainBake ui_bake_;

  UiSlotsTerrain slots_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainSlotsCreate) -
      static_cast<int>(data::VboIdMain::kTerrainFlatten) + 1
      > ui_event_handler_;

  void Cancel();

  void Apply();

  void CancelTransform();

  void ApplyTransform();

  void SetOriginPosition(GLuint pressed_id);

  glm::vec3 transform_axis_ = glm::vec3(1.0f);
  bool mouse_process_ = false;
  int pressed_mouse_key_ = 0;

  glm::vec4 cursor_pos_ = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
};

namespace terrain {

void BindCallbacksDefault();

void ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

/*
MMB - 3d rotation around camera lookAt_origin, snap with ALT
MMB+Shift - move in set up-right plane

1-2-3-4 modes: square, circle, lasso, toggle
LMB - selecting (CursorPosCallback -> add new points in radius)
LMB+Shift - extend selection
LMB+Ctrl - deselect
LMB+Ctrl+Shift - toggle selection

RMB_SHIFT_PRESS - set origin
 * */

void CursorPosCallback_Mmb(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_MmbShift(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_LmbShift(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_LmbCtrl(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_LmbCtrlShift(
    GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_RmbShift(
    GLFWwindow* window, double xpos, double ypos);



void BindCallbacksTransform();

void ScrollCallbackTransform(
    GLFWwindow* window, double xoffset, double yoffset);

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

} // namespace terrain

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_
