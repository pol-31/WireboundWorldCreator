#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINMODE_H_

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "IEditMode.h"
#include "../core/Ui.h"
#include "../common/Vbos.h"

void TerrainModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void TerrainModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void TerrainModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class TerrainMode final : public IEditMode {
 public:
  explicit TerrainMode(SharedResources& shared_resources);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

  void Bake();

  void SwitchSmooth();

 protected:
  friend void TerrainModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void TerrainModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void TerrainModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiStaticSprite btn_bake_;
  UiStaticSprite btn_smooth_;
  UiSlider slider_size_;
  UiSlider slider_falloff_;

  // set "true" to initialize at first Render() call
  bool need_to_update_uniforms_{true};

  bool smooth_mode_{false};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINMODE_H_
