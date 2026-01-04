#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_

#define GLFW_INCLUDE_NONE

#include "../common/MouseTransform.h"
#include "../common/Vbos.h"
#include "../core/Tile.h"
#include "../core/UiComplex.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "IUiMode.h"
#include "UiTerrainWindows.h"

struct GLFWwindow;

class UiTerrainMode final : public IUiMode {
 public:
  UiTerrainMode(UiSharedResources& ui_shared_resources, UiSlots& ui_slots,
                WindowQueue& window_queue, TextRenderer& text_renderer,
                Tile& cur_tile, UiConfigWindow& ui_config_window);

  void Render() override;

  void RenderPicking() override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  int GetPrerenderTextIdStart() const noexcept override;

  int GetPrerenderTextIdEnd() const noexcept override;

  void RenderWorld() override;

  void RenderPickingWorld() override;

  void CancelTransform();

  void ApplyTransform();

  void UpdateTransform();

  void Reset();

  UiDynamicSprite btn_update_;
  UiDynamicSprite btn_reset_;
  UiDynamicSprite btn_bake_;
  UiToggle4 tg_flatten_;

  UiTerrainBake ui_bake_;
  std::vector<BaseInstanceData> layers_;
  UiSlots& ui_slots_;
  UiEditTerrain ui_edit_;  // ! after ui_slots
  UiSelection ui_selection_;

  MouseTransform mouse_transform_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kTerrainEditNoiseHmap) -
                 static_cast<int>(data::VboIdMain::kTerrainFlatten) + 1>
      ui_event_handler_;
};

namespace terrain {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

/**
MMB - 3d rotation around camera lookAt_origin, snap with ALT
MMB+Shift - move in set up-right plane

1-2-3-4 modes: square, circle, lasso, toggle
LMB - selecting (CursorPosCallback -> add new points in radius)
LMB+Shift - extend selection
LMB+Ctrl - deselect
LMB+Ctrl+Shift - toggle selection

RMB_SHIFT_PRESS - set origin
 * */

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_RmbShift(GLFWwindow* window, double xpos, double ypos);

void BindCallbacksTransform(bool init_transform);

void MouseButtonCallbackTransform(GLFWwindow* window, int button, int action,
                                  int mods);

void KeyCallbackTransform(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

void CursorPosCallback_G(GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_G_NonSelected(GLFWwindow* window, double xpos,
                                     double ypos);

void CursorPosCallback_R(GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_S(GLFWwindow* window, double xpos, double ypos);

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods);

void MouseButtonCallback_RmbShift(GLFWwindow* window, int button, int action,
                                  int mods);

}  // namespace terrain

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_
