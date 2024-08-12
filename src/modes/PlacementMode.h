#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"

void PlacementModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void PlacementModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void PlacementModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class PlacementMode final : public IEditMode {
 public:
  PlacementMode(SharedResources& shared_resources);

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

  UiButton btn_place_trees_;
  UiButton btn_place_bushes_;
  UiButton btn_place_tall_grass_;
  UiButton btn_place_undergrowth_;
  UiSlider slider_color_;
  UiSlider slider_size_;
  UiSlider slider_falloff_;

  float color_factor_{1.0f};
  float size_factor_{1.0f};
  float falloff_factor_{1.0f};

  bool slider_color_pressed_{false};
  bool slider_size_pressed_{false};
  bool slider_falloff_pressed_{false};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_
