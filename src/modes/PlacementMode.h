#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"
#include "../common/Paths.h"

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

  void PlaceTrees();
  void PlaceBushes();
  void PlaceTallGrass();
  void PlaceUndergrowth();

  void BindCallbacks() override;

 protected:
  friend void PlacementModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void PlacementModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void PlacementModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  void DrawPixels(std::uint32_t pressed_id);

  void UpdateColorSliderPos(glm::vec2 position);
  void UpdateSizeSliderPos(glm::vec2 position);
  void UpdateFalloffSliderPos(glm::vec2 position);

  UiButton btn_place_trees_;
  UiButton btn_place_bushes_;
  UiButton btn_place_tall_grass_;
  UiButton btn_place_undergrowth_;

  /// slider progress stored both on shader uniform and on UiSlider,
  /// but here there's no sense to keep it
  UiSlider slider_color_;
  UiSlider slider_size_;
  UiSlider slider_falloff_;

  Shader shader_draw_;

  bool slider_color_pressed_{false};
  bool slider_size_pressed_{false};
  bool slider_falloff_pressed_{false};

  /// rendered with shaders - not black/white height map
  bool preview_mode_{false};

  /// to hold mouse button and draw
  bool draw_{false};

  /// prevents an accumulation for falloff effect, so holding mouse on the same
  /// position won't affect it
  std::uint32_t last_modified_point_{static_cast<std::uint32_t>(-1.0f)};

  GLuint shader_image_unit_{0}; //TODO: Details.h
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_
