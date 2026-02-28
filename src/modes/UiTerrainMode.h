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
#include "UiEditTerrain.h"
#include "UiTerrainBake.h"

struct GLFWwindow;

class UiTerrainMode final : public IUiMode {
 public:
  UiTerrainMode(UiRenderData& render_data, UiSlots& ui_slots,
                WindowQueue& window_queue, TextRenderer& text_renderer,
                UiEditSlots& ui_edit_slots, UiEditConfigSlCfg& value_config,
                Tile& cur_tile, UiConfigWindow& ui_config_windowm);

  void Render(TileRenderer* tile_renderer,
              UiRenderer__Deprecated* ui_renderer) override;

  void RenderPicking(TileRenderer* tile_renderer,
                     UiRenderer__Deprecated* ui_renderer) override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  void PrerenderText(TextRenderer* text_renderer) override;

  void CancelTransform();

  void ApplyTransform();

  void UpdateTransform();

  void Reset();

  UiSprite sp_mode_;
  UiSprite btn_update_;
  UiSprite btn_reset_;
  UiSprite btn_bake_;

  UiTerrainBake ui_bake_;
  UiSlots& ui_slots_;
  UiEditTerrain ui_edit_;  // ! after ui_slots
  UiSelection ui_selection_;

  /// G / S / R transformation started -> moving the whole layer,
  /// not selected only (everything considered to be selected)
  bool nothing_selected_ = true;

  MouseTransform mouse_transform_;
  UiEventHandler ui_event_handler_;
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
