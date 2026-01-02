#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_

#include <glad/glad.h>

#include "../common/MouseTransform.h"
#include "../common/Vbos.h"
#include "../core/UiSelection.h"
#include "../core/UiSlots.h"
#include "IUiMode.h"
#include "UiEditWater.h"

/// two modes: ocean (Ifft), river (Swelbm)
class UiWaterMode final : public IUiMode {
 public:
  UiWaterMode(UiSharedResources& ui_shared_resources, UiSlots& ui_slots,
              WindowQueue& window_queue, TextRenderer& text_renderer,
              UiConfigWindow& ui_config_window, ModelManager& mdl_manager);

  void Render() override;

  void RenderPicking() override;

  void RenderMapPoints();

  void RenderPickingMapPoints();

  void Setup() override;

  void BindDefaultCallbacks() override;

  int GetPrerenderTextIdStart() const noexcept override;

  int GetPrerenderTextIdEnd() const noexcept override;

  void RenderWorld() override;

  void RenderPickingWorld() override;

  void AddPlacementPoint(GLuint pressed_id);

  void HandleSelection(const std::set<GLuint>& selected_ids);

  void UpdateRivers();

  void CancelTransform();

  void ApplyTransform();

  void AddJoints(GLuint id);

  //  void UpdateOcean();
  //
  //  bool ConfigModified();

  UiDynamicSprite btn_bake_ocean_;
  UiDynamicSprite btn_bake_river_;
  UiDynamicSprite btn_update_;
  UiSelectedSprite sp_selected_mode_;

  UiSlots& ui_slots_;

  std::vector<BaseInstanceData> instances_ocean_;
  std::vector<BaseInstanceData> instances_river_;
  UiEditOcean ui_edit_ocean_;
  UiEditRiver ui_edit_river_;

  bool ocean_ = true;  // otherwise river

  UiSelection ui_selection_;
  bool anything_selected_ = false;

  glm::vec3 temp_translate_ = glm::vec3(0.0f);

  ModelManager& mdl_manager_;
  MouseTransform mouse_transform_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kWaterRiver) -
                 static_cast<int>(data::VboIdMain::kWaterUpdate) + 1>
      ui_event_handler_;
};

namespace water {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos);

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods);

void CursorPosCallback_LmbSelected(GLFWwindow* window, double xpos,
                                   double ypos);

void MouseButtonCallback_LmbSelected(GLFWwindow* window, int button, int action,
                                     int mods);

}  // namespace water

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
