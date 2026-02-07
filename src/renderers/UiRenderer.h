#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_UIRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_UIRENDERER_H_

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../ui/UiRenderData.h"
#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"
#include "../common/models/ModelManager.h"  // temp
#include "../core/TileRenderer.h"
#include "../core/Ui.h"
#include "../core/UiButtons.h"
#include "../core/UiConfigWindow.h"
#include "../core/UiGrid.h"
#include "../core/UiLayerWireframe.h"
#include "../core/UiMenu.h"
#include "../core/UiModal.h"
#include "../core/UiSlots.h"
#include "../core/UiWorldOrigin.h"
#include "../core/WindowQueue.h"
#include "../modes/UiBiomesMode.h"
#include "../modes/UiEdit.h"
#include "../modes/UiEditShared.h"
#include "../modes/UiObjectsMode.h"
#include "../modes/UiPlacementMode.h"
#include "../modes/UiPlayerMode.h"
#include "../modes/UiTerrainMode.h"
#include "../modes/UiTilesMode.h"
#include "../modes/UiWaterMode.h"

class Camera;

class UiRenderer {
 public:
  UiRenderer(GlfwContext& global_glfw_data_, TileRenderer& tile_renderer,
             const Camera* camera);

  UiRenderer(UiRenderer&& other) = delete;
  UiRenderer(const UiRenderer& other) = delete;
  UiRenderer& operator=(UiRenderer&& other) = delete;
  UiRenderer& operator=(const UiRenderer& other) = delete;

  void Render(TileRenderer* tile_renderer);

  void RenderPicking(TileRenderer* tile_renderer);

  void Press(int key, int action);

  void Parse();

  void Serialize();

  void SetupGlobalData();

  void RenderWorldOrigin(glm::vec4 position, glm::vec4 color);

  void RenderAxis(float scale);

  [[nodiscard]] const UiRenderData& GetRenderData() const {
    return render_data_;
  }

 private:
  void Init();

  UiRenderData render_data_;
  ModelManager mdl_manager_;

  debug::UiDebugger ui_debugger_;

  WindowQueue windows_;
  TextRenderer text_renderer_;

  UiSlots ui_slots_;
  UiConfigWindow ui_config_window_;
  UiEditSlots ui_edit_slots_;
  UiEditConfigSlCfg ui_edit_1_;
  UiEditConfigSlTxt ui_edit_2_;

  IUiMode* cur_mode_ = nullptr;
  UiTerrainMode terrain_;
  UiWaterMode water_;
  UiBiomesMode biomes_;
  UiObjectsMode objects_;
  UiPlacementMode placement_;
  UiTilesMode tiles_;
  UiPlayerMode player_;

  UiMenu ui_menu_;
  UiSettings ui_settings_;

  bool debug_ui_prev_ = false;
  bool render_menu_ = false;

  UiButtons ui_buttons_;
  UiGrid ui_grid_;

public:
  UiTipWindow ui_tip_;

  UiConfirmation ui_confirmation_;
  UiWarning ui_warning_;

  UiLoading ui_loading_;
  UiCompass ui_compass_;

  UiLayerWireframe ui_layer_wireframe_;
  UiWorldOrigin ui_world_origin_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_UIRENDERER_H_
