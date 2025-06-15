#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "IUiMode.h"
#include "../common/Vbos.h"

#include "UiTerrainGenerator.h"

#include "../core/Tile.h"

class UiTerrainMode final : public IUiMode {
 public:
  explicit UiTerrainMode(UiSharedResources& ui_shared_resources,
                         Tile& cur_tile);

  void Render() override;
  void RenderPicking() override;
  data::TextId Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

  glm::uvec2 GetPrerenderTextIds() const noexcept override;

 protected:
  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiStaticSprite sprite_flatten_;

  UiStaticSprite btn_update_;
  UiStaticSprite btn_regenerate_;

  UiSliderV slider_size_;
  UiSliderV slider_falloff_;

  UiToggle toggle_flatten_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainFalloffIcon) -
      static_cast<int>(data::VboIdMain::kTerrainFlatten) + 1
      > ui_event_handler_;

  bool smooth_mode_{false};

  UiTerrainGenerator ui_terrain_generator_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_
