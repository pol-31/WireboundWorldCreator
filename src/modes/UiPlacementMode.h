#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_

#include <glad/glad.h>

#include "IUiMode.h"
#include "../common/Vbos.h"
#include "../core/Ui.h"
#include "../common/Paths.h"
#include "../common/Shader.h"
#include "../common/Texture.h"

class UiPlacementMode final : public IUiMode {
 public:
  explicit UiPlacementMode(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      const Paths& paths);

  void Render() override;
  void RenderPicking() override;

  void BindCallbacks() override;

  /// draw or preview
  void BtnChangeMode();

  void BtnTrees();

  void BtnBushes();

  void BtnTallGrass();

  void BtnUndergrowth();

 protected:
  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  void DrawPixels(std::uint32_t prev_id, std::uint32_t last_id);

  void PlaceLastModified() const;

  // for serializing
  static void InitHeightMap(std::string_view path, Texture& texture);

  UiStaticSprite btn_trees_;
  UiStaticSprite btn_bushes_;
  UiStaticSprite btn_tall_grass;
  UiStaticSprite btn_undergrowth_;
  UiStaticSprite btn_change_mode_;

  UiSliderV slider_color_;
  UiSliderV slider_size_;
  UiSliderV slider_falloff_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kPlacementChangeMode) -
      static_cast<int>(data::VboIdMain::kPlacementPlacementMode) + 1
      > ui_event_handler_;


  // --- REFACTOR? idk about everything below ---
  Texture new_draw_layer_; // TODO: copy explanation from shader to here

  Shader shader_draw_;

  /// rendered with shaders - not black/white height map
  bool preview_mode_{false};

  /// to hold mouse button and draw
  bool draw_{false};

  /// prevents an accumulation for falloff effect, so holding mouse on the same
  /// position won't affect it
  std::uint32_t last_modified_point_{static_cast<std::uint32_t>(-1.0f)};

  GLuint last_modified_placement_;

  Texture map_placement_trees_;
  Texture map_placement_bushes_;
  Texture map_placement_tall_grass_;
  Texture map_placement_undergrowth_;

  Texture* cur_placement_mode_tex_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIPLACEMENTMODE_H_
