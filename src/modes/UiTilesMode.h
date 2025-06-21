#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_

#include "IUiMode.h"
#include "../common/Vbos.h"

class UiTilesMode final : public IUiMode {
 public:
  explicit UiTilesMode(UiSharedResources& ui_shared_resources);

  void Render() override;
  void RenderPicking() override;
  data::TextId Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

 protected:
  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiStaticSprite btn_up_;
  UiStaticSprite btn_down_;
  UiStaticSprite btn_left_;
  UiStaticSprite btn_right_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTilesDown) -
      static_cast<int>(data::VboIdMain::kTilesUiTilesMode) + 1
      > ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_
