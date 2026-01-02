#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_

#include <glad/glad.h>

#include "../common/Shader.h"
#include "../common/Texture.h"
#include "../common/Vbos.h"
#include "../common/models/ModelManager.h"
#include "../core/Ui.h"
#include "../core/UiComplex.h"
#include "../core/UiSelection.h"
#include "IUiMode.h"

class UiPlacementMode final : public IUiMode {
 public:
  UiPlacementMode(UiSharedResources& ui_shared_resources,
                  WindowQueue& window_queue, ModelManager& mdl_manager);

  void Render() override;

  void RenderPicking() override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  int GetPrerenderTextIdStart() const noexcept override;

  int GetPrerenderTextIdEnd() const noexcept override;

  void RenderWorld() override;

  void RenderPickingWorld() override;

  void SetPlacementMode(Texture* tex_placement);

  [[nodiscard]] bool IsPreviewMode() const noexcept { return preview_mode_; }

  UiDynamicSprite btn_trees_;
  UiDynamicSprite btn_bushes_;
  UiDynamicSprite btn_tall_grass;
  UiDynamicSprite btn_undergrowth_;
  UiDynamicSprite btn_asphalt_;
  UiDynamicSprite btn_gravel_;
  UiDynamicSprite btn_soil_;
  UiDynamicSprite btn_change_mode_;  /// toggle/swap
  UiSelectedSprite sp_selected_mode_;

  UiSelection ui_selection_;
  ModelManager& mdl_manager_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kPlacementChangeMode) -
                 static_cast<int>(data::VboIdMain::kPlacementPlacementMode) + 1>
      ui_event_handler_;

  Texture tex_placement_trees_;
  Texture tex_placement_bushes_;
  Texture tex_placement_tall_grass_;
  Texture tex_placement_undergrowth_;
  Texture tex_placement_asphalt_;
  Texture tex_placement_gravel_;
  Texture tex_placement_soil_;

  Texture* tex_cur_placement_ = nullptr;
  bool preview_mode_ = false;

 private:
  void SetDrawTexture(Texture* tex_placement);

  void TogglePlacement();
};

namespace placement {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods);

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos);

}  // namespace placement

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_
