#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_

#include "../common/Vbos.h"
#include "IUiMode.h"

class UiTilesMode final : public IUiMode {
 public:
  explicit UiTilesMode(UiSharedResources& ui_shared_resources,
                       WindowQueue& window_queue);

  void Render() override;
  void RenderPicking() override;

  void Setup() override;

  void BindDefaultCallbacks() override {}

  int GetPrerenderTextIdStart() const noexcept override { return 0; }

  int GetPrerenderTextIdEnd() const noexcept override { return 0; }

  void RenderWorld() override;

  void RenderPickingWorld() override;

 protected:
  static void ScrollCallback(GLFWwindow* window, double xoffset,
                             double yoffset);

  static void MouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods);

  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

  UiStaticSprite btn_up_;
  UiStaticSprite btn_down_;
  UiStaticSprite btn_left_;
  UiStaticSprite btn_right_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kTilesDown) -
                 static_cast<int>(data::VboIdMain::kTilesTilesMode) + 1>
      ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_
