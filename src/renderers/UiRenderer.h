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

#ifndef NDEBUG
#include "../common/UiDebugger.h"
#endif // NDEBUG

class UiRenderer {
 public:
  UiRenderer(const Paths& paths,
             GlobalGlfwCallbackData& global_glfw_data_,
             TileRenderer& tile_renderer);

  void Render(data::TextId description_id);

  data::TextId Hover(GLuint pressed_id);

  void RenderPicking(data::TextId description_id);

  void Parse();

  void Serialize();

  void SetupGlobalData();

 private:
  void Init();

  UiSharedResources ui_shared_resources_;

  debug::UiDebugger ui_debugger_;

  IUiMode* cur_mode_{nullptr};
  UiTerrainMode terrain_;
  UiWaterMode water_;
  UiFencesMode fences_;
  UiRoadsMode roads_;
  UiBiomesMode biomes_;
  UiObjectsMode objects_;
  UiPlacementMode placement_;
  UiTilesMode tiles_;

  TextRenderer text_renderer_;

  Menu menu_;

  bool debug_ui_prev_{false};

  bool render_menu_{false};

  UiLoading ui_loading_;
  UiConfirmation ui_confirmation_;
  UiDynamicSprite compass_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_UIRENDERER_H_
