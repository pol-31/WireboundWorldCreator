#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "IUiMode.h"
#include "../common/Vbos.h"

#include "UiTerrainGenerator.h"

#include "../core/Tile.h"
#include "../core/UiSlots.h"
#include "../core/UiComplex.h"

class UiTerrainMode final : public IUiMode {
 public:
  explicit UiTerrainMode(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer,
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

  UiStaticSprite btn_update_;
  UiStaticSprite sprite_flatten_;
  UiToggle toggle_flatten_;

  UiSliderV slider_size_;
  UiSliderV slider_falloff_;

  UiStaticSprite btn_bake_;
  UiTerrainBake ui_bake_;

  UiSlotsTerrain slots_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainSlotsFlipPointEdgeFace) -
      static_cast<int>(data::VboIdMain::kTerrainFlatten) + 1
      > ui_event_handler_;

  UiTerrainGenerator ui_terrain_generator_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINMODE_H_
