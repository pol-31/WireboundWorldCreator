#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_

#include <glad/glad.h>

#include "../common/MapPoints.h"
#include "../common/MouseTransform.h"
#include "../common/Shader.h"
#include "../common/Texture.h"
#include "../common/Vbos.h"
#include "../common/models/ModelManager.h"
#include "../core/Ui.h"
#include "../core/UiComplex.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "IUiMode.h"
#include "UiEditRoads.h"

class UiPlacementMode final : public IUiMode {
 public:
  UiPlacementMode(UiSharedResources& ui_shared_resources, UiSlots& ui_slots,
                  WindowQueue& window_queue, UiEditSlots& ui_edit_slots,
                  UiEditConfigSlTxt& value_config, ModelManager& mdl_manager);

  void Render() override;

  void RenderPicking() override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  void SetPlacementMode(Texture* tex_placement, int id);

  [[nodiscard]] bool IsPreviewMode() const noexcept { return preview_mode_; }

  void HandleSelection(const std::set<GLuint>& selected_ids);

  void CancelTransform();

  void ApplyTransform();

  void OnSelectedSlotChanged();

  void TogglePlacement();

  Texture* GetPlacementTree();
  Texture* GetPlacementBushes();
  Texture* GetPlacementTallGrass();
  Texture* GetPlacementUndergrowth();

  UiDynamicSprite btn_trees_;
  UiDynamicSprite btn_bushes_;
  UiDynamicSprite btn_tall_grass;
  UiDynamicSprite btn_undergrowth_;
  UiDynamicSprite btn_change_mode_;  /// toggle/swap
  UiSelectedSprite sp_selected_mode_;

  UiSlots& ui_slots_;
  UiEditRoads ui_edit_;

  UiSelection ui_selection_;
  bool anything_selected_ = false;
  ModelManager& mdl_manager_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kPlacementChangeMode) -
                 static_cast<int>(data::VboIdMain::kPlacementPlacementMode) + 1>
      ui_event_handler_;

  Texture* tex_cur_placement_ = nullptr;
  bool preview_mode_ = false;

  MapPoints map_points_;
  MouseTransform mouse_transform_;
};

namespace placement {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods);

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos);

void KeyCallback_LmbSelected(GLFWwindow* window, int key, int scancode,
                             int action, int mods);

void CursorPosCallback_LmbSelected(GLFWwindow* window, double xpos,
                                   double ypos);

void MouseButtonCallback_LmbSelected(GLFWwindow* window, int button, int action,
                                     int mods);

}  // namespace placement

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_
