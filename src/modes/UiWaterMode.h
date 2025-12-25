#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_

#include <glad/glad.h>

#include "IUiMode.h"
#include "../common/Vbos.h"
#include "../core/UiSlots.h"
#include "../core/UiSelection.h"
#include "UiEditWater.h"

/// two modes: ocean (Ifft), river (Swelbm)
class UiWaterMode final : public IUiMode {
 public:
  UiWaterMode(
      UiSharedResources& ui_shared_resources,
      UiSlots& ui_slots,
      WindowQueue& window_queue,
      TextRenderer& text_renderer,
      UiConfigWindow& ui_config_window);

  void Render() override;

  void RenderPicking() override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  int GetPrerenderTextIdStart() const noexcept override;

  int GetPrerenderTextIdEnd() const noexcept override;

//  void UpdateOcean();
//
//  bool ConfigModified();

  UiDynamicSprite btn_bake_ocean_;
  UiDynamicSprite btn_bake_river_;
  UiDynamicSprite btn_update_;
  UiDynamicSprite sp_selected_mode_;

  std::vector<BaseInstanceData> instances_;
  UiSlots& ui_slots_;
  UiEditOcean ui_edit_;
  UiSelection ui_selection_;
};

namespace water {

void ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos);

void MouseButtonCallback_Lmb(
    GLFWwindow* window, int button, int action, int mods);

} // namespace water

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
