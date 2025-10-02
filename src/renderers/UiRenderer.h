#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_UIRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_UIRENDERER_H_

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "../common/Paths.h"
#include "../core/Ui.h"

#include "../core/Menu.h"
#include "../common/TextRenderer.h"
#include "../core/TileRenderer.h"
#include "../modes/AllUiModes.h"

#include "../core/WindowQueue.h"

#include "../core/UiLayerWireframe.h"
#include "../core/UiWorldOrigin.h"
#include "../core/UiButtons.h"

#ifndef NDEBUG
#include "../common/UiDebugger.h"
#endif // NDEBUG

class CameraHandler;

class UiRenderer {
 public:
  UiRenderer(const Paths& paths,
             GlobalGlfwCallbackData& global_glfw_data_,
             TileRenderer& tile_renderer,
             const CameraHandler* camera);

  /// delete everything, just in case we missed something
  UiRenderer(UiRenderer&& other) = delete;
  UiRenderer(const UiRenderer& other) = delete;
  UiRenderer& operator=(UiRenderer&& other) = delete;
  UiRenderer& operator=(const UiRenderer& other) = delete;

  void Render();

  void RenderPicking();

  void Press(int key, int action);

  void Parse();

  void Serialize();

  void SetupGlobalData();

  //TODO: text should be prerendered in menu_prerender_texture by now
  void AskForConfirmation(
      data::TextId text, std::function<void()>&& callable);

 UiLayerWireframe& GetUiLayerWireframe() {
   return ui_layer_wireframe_;
 }

 void RenderWorldOrigin(glm::vec4 position, glm::vec4 color);

 private:
  void Init();

  UiSharedResources ui_shared_resources_;

  debug::UiDebugger ui_debugger_;

  WindowQueue windows_;
  TextRenderer text_renderer_;

  IUiMode* cur_mode_{nullptr};
  UiTerrainMode terrain_;
  UiWaterMode water_;
  UiFencesMode fences_;
  UiRoadsMode roads_;
  UiBiomesMode biomes_;
  UiObjectsMode objects_;
  UiPlacementMode placement_;
  UiTilesMode tiles_;


  Menu menu_;
  UiSettings ui_settings_;
  UiConfirmation ui_confirmation_;
  UiCaution ui_caution_;
  UiFile ui_file_;
  UiTipWindow ui_tip_;

  UiLoading ui_loading_;
  UiCompass ui_compass_;

  bool debug_ui_prev_{false};
  bool render_menu_{false};

  UiLayerWireframe ui_layer_wireframe_;
  UiWorldOrigin ui_world_origin_;

  UiButtons ui_buttons_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_UIRENDERER_H_
