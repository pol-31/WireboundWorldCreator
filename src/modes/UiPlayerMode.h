#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLAYERMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLAYERMODE_H_

#include <map>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../common/Vbos.h"
#include "../common/models/ModelManager.h"
#include "../core/Tile.h"
#include "../core/UiComplex.h"
#include "../core/UiSelection.h"
#include "IUiMode.h"

/// everything's public, otherwise need to make too much callback friends
class UiPlayerMode final : public IUiMode {
 public:
  explicit UiPlayerMode(UiSharedResources& ui_shared_resources,
                        WindowQueue& window_queue, TextRenderer& text_renderer,
                        Tile& cur_tile, ModelManager& mdl_manager);

  void Render() override;

  void RenderPicking() override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  void HandleSelection(const std::set<GLuint>& selected_ids);

  UiDynamicSprite sp_hp_;

  UiObjectInfo ui_obj_info_;
  UiPlayerMap ui_map_;

  UiSelection ui_selection_;
  ModelManager& mdl_manager_;

  /*  UiEventHandler<
        static_cast<int>(data::VboIdMain::kTerrainSlotsCreate) -
        static_cast<int>(data::VboIdMain::kTerrainFlatten) + 1
        > ui_event_handler_;*/
};

namespace player {

void BindCallbacksDefault();

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void MouseButtonCallback_Selection(GLFWwindow* window, int button, int action,
                                   int mods);

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos);

void CursorPosCallback_Mmb(GLFWwindow* window, double xpos, double ypos);

}  // namespace player

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLAYERMODE_H_
