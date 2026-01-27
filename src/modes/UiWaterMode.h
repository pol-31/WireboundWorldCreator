#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_

#include <glad/glad.h>

#include "../common/MapPoints.h"
#include "../common/MouseTransform.h"
#include "../common/Vbos.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "IUiMode.h"
#include "UiEditOcean.h"
#include "UiEditRiver.h"

class UiWaterMode final : public IUiMode {
 public:
  UiWaterMode(UiSharedResources& ui_shared_resources, UiSlots& ui_slots,
              WindowQueue& window_queue, TextRenderer& text_renderer,
              UiEditSlots& ui_edit_slots, UiEditConfigSlCfg& value_config_ocean,
              UiEditConfigSlTxt& value_config_river,
              UiConfigWindow& ui_config_window, ModelManager& mdl_manager);

  void Render() override;

  void RenderPicking() override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  void HandleSelection(const std::set<GLuint>& selected_ids);

  void UpdateRivers();

  void CancelTransform();

  void ApplyTransform();

  void OnSelectedSlotChanged(bool is_ocean);

  UiSprite btn_bake_ocean_;
  UiSprite btn_bake_river_;
  UiSprite btn_update_;
  UiSelectedSprite sp_selected_mode_;

  UiSlots& ui_slots_;
  UiEditOcean ui_edit_ocean_;
  UiEditRiver ui_edit_river_;

  UiSelection ui_selection_;
  bool anything_selected_ = false;

  MouseTransform mouse_transform_;
  MapPoints map_points_;
  UiEventHandler ui_event_handler_;
};

namespace water {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos);

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods);

void KeyCallback_LmbSelected(GLFWwindow* window, int key, int scancode,
                             int action, int mods);

void CursorPosCallback_LmbSelected(GLFWwindow* window, double xpos,
                                   double ypos);

void MouseButtonCallback_LmbSelected(GLFWwindow* window, int button, int action,
                                     int mods);

void CursorPosCallback_G(GLFWwindow* window, double xpos, double ypos);

void MouseButtonCallbackTransform(GLFWwindow* window, int button, int action,
                                  int mods);

void KeyCallbackTransform(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

}  // namespace water

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
