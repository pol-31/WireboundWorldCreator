#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINMODE_H_

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "IEditMode.h"
#include "../core/Ui.h"
#include "../vbos/UiDataMain.h"

void TerrainModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void TerrainModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void TerrainModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class TerrainMode final : public IEditMode {
 public:
  TerrainMode(SharedResources& shared_resources)
      : IEditMode(shared_resources),
        btn_bake_(shared_resources.static_sprite_shader,
                  "bake terrain (update all other components"
                  " like water, roads, etc)",
                  GetUiDataMainOffset(UiVboDataMainId::kTerrainUpdate),
                  *this),
        btn_smooth_(shared_resources.static_sprite_shader,
                    "enable smooth mode (flatten the terrain)",
                    GetUiDataMainOffset(UiVboDataMainId::kTerrainSmooth),
                    *this),
        slider_brush_size_(shared_resources.static_sprite_shader, "",
                           {shared_resources.static_sprite_shader,
                            "min size", GetUiDataMainOffset(UiVboDataMainId::kUiSliderRadiusMin)},
                           {shared_resources.static_sprite_shader,
                            "max size", GetUiDataMainOffset(UiVboDataMainId::kUiSliderRadiusMax)},
                           {shared_resources.static_sprite_shader,
                            "size slider track", GetUiDataMainOffset(UiVboDataMainId::kUiSliderRadiusArea)},
                           {shared_resources.slider_handle_shader,
                            "size slider handle", GetUiDataMainOffset(UiVboDataMainId::kUiSliderRadiusSlider), size_factor_}),
        slider_brush_falloff_(shared_resources.static_sprite_shader, "",
                              {shared_resources.static_sprite_shader,
                               "min falloff", GetUiDataMainOffset(UiVboDataMainId::kUiSliderFalloffMin)},
                              {shared_resources.static_sprite_shader,
                               "max falloff", GetUiDataMainOffset(UiVboDataMainId::kUiSliderFalloffMax)},
                              {shared_resources.static_sprite_shader,
                               "falloff slider track", GetUiDataMainOffset(UiVboDataMainId::kUiSliderFalloffArea)},
                              {shared_resources.slider_handle_shader,
                               "falloff slider handle", GetUiDataMainOffset(UiVboDataMainId::kUiSliderFalloffSlider),
                               falloff_factor_}) {}

  void Render() override;
  void BindCallbacks() override;

  void Bake() {
    std::cout << "baked" << std::endl;
  }

  void SwitchSmooth() {
    smooth_mode_ = !smooth_mode_;
    std::cout << "smooth mode enabled: " << std::boolalpha << smooth_mode_
              << std::noboolalpha << std::endl;
  }

 protected:
  friend void TerrainModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void TerrainModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void TerrainModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiButtonTerrainModeBake btn_bake_;
  UiButtonTerrainModeSmooth btn_smooth_;
  UiSlider slider_brush_size_;
  UiSlider slider_brush_falloff_;

  float size_factor_{1.0f};
  float falloff_factor_{1.0f};

  bool slider_brush_size_pressed_{false};
  bool slider_brush_falloff_pressed_{false};

  bool smooth_mode_{false};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINMODE_H_
