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
  explicit UiPlayerMode(UiRenderData& render_data, WindowQueue& window_queue,
                        TextRenderer& text_renderer, Tile& cur_tile,
                        ModelManager& mdl_manager);

  void Render(TileRenderer* tile_renderer,
              UiRenderer__Deprecated* ui_renderer) override;

  void RenderPicking(TileRenderer* tile_renderer,
                     UiRenderer__Deprecated* ui_renderer) override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  void PrerenderText(TextRenderer* text_renderer) override;

  void HandleSelection(const std::set<GLuint>& selected_ids);

  UiSprite sp_mode_;
  UiSprite sp_hp_;

  UiObjectInfo ui_obj_info_;
  UiPlayerMap ui_map_;

  UiSelection ui_selection_;
  ModelManager& mdl_manager_;
  UiRenderData& render_data_;  // TODO: temp (need to setup camera)
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
