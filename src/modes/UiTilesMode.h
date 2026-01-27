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

 protected:
  static void ScrollCallback(GLFWwindow* window, double xoffset,
                             double yoffset);

  static void MouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods);

  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

  UiSprite btn_up_;
  UiSprite btn_down_;
  UiSprite btn_left_;
  UiSprite btn_right_;

  UiEventHandler ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_
