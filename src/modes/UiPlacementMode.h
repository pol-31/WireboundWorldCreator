#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_

#include <glad/glad.h>

#include "../common/MapPoints.h"
#include "../common/MouseTransform.h"
#include "../common/Vbos.h"
#include "../common/models/ModelManager.h"
#include "../core/Ui.h"
#include "../core/UiComplex.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "../render/Shader.h"
#include "../render/Texture.h"
#include "IUiMode.h"
#include "UiEditRoads.h"

class UiPlacementMode final : public IUiMode {
 public:
  UiPlacementMode(UiRenderData& render_data, UiSlots& ui_slots,
                  WindowQueue& window_queue, UiEditSlots& ui_edit_slots,
                  UiEditConfigSlTxt& value_config, ModelManager& mdl_manager);

  void Render(TileRenderer* tile_renderer,
              UiRenderer__Deprecated* ui_renderer) override;

  void RenderPicking(TileRenderer* tile_renderer,
                     UiRenderer__Deprecated* ui_renderer) override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  void PrerenderText(TextRenderer* text_renderer) override;

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

  UiSprite sp_mode_;
  UiSprite btn_trees_;
  UiSprite btn_bushes_;
  UiSprite btn_tall_grass;
  UiSprite btn_undergrowth_;
  UiSprite btn_change_mode_;  /// toggle/swap
  UiSelectedSprite sp_selected_mode_;

  UiSlots& ui_slots_;
  UiEditRoads ui_edit_;

  UiSelection ui_selection_;
  bool anything_selected_ = false;
  ModelManager& mdl_manager_;

  UiEventHandler ui_event_handler_;

  Texture* tex_cur_placement_ = nullptr;
  bool preview_mode_ = false;

  MapPoints map_points_;
  MouseTransform mouse_transform_;
  UiRenderData& render_data_;  // TODO: temp (need to set placement)
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
