#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"
#include "../common/Paths.h"
#include "../common/Shader.h"
#include "../common/Texture.h"

void PlacementModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void PlacementModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void PlacementModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class PlacementMode final : public IEditMode {
 public:
  explicit PlacementMode(SharedResources& shared_resources,
                         const Paths& paths);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

  /// draw or preview
  void SwitchViewMode();

 protected:
  friend void PlacementModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void PlacementModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void PlacementModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  void DrawPixels(std::uint32_t prev_id, std::uint32_t last_id);

  void PlaceTrees();
  void PlaceBushes();
  void PlaceTallGrass();
  void PlaceUndergrowth();

  void PlaceLastModified() const;

  UiStaticSprite btn_place_trees_;
  UiStaticSprite btn_place_bushes_;
  UiStaticSprite btn_place_tall_grass_;
  UiStaticSprite btn_place_undergrowth_;

  /// slider progress stored both on shader uniform and on UiSlider,
  /// but here there's no sense to keep it
  UiSliderV slider_color_;
  UiSliderV slider_size_;
  UiSliderV slider_falloff_;

  // set "true" to initialize at first Render() call
  bool need_to_update_uniforms_{true};

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
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_
