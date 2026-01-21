#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIBIOMESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIBIOMESMODE_H_

#include "../common/MapPoints.h"
#include "../common/MouseTransform.h"
#include "../common/Vbos.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "IUiMode.h"
#include "UiEditBiomes.h"

class UiBiomesMode final : public IUiMode {
 public:
  UiBiomesMode(UiSharedResources& ui_shared_resources, UiSlots& ui_slots,
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

  UiDynamicSprite sp_biome_;

  UiSlots& ui_slots_;
  UiEditBiomes ui_edit_;

  UiSelection ui_selection_;
  bool anything_selected_ = false;

  MapPoints map_points_;
  MouseTransform mouse_transform_;
};

namespace biomes {

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
}  // namespace biomes

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIBIOMESMODE_H_
