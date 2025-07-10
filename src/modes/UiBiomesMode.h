#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIBIOMESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIBIOMESMODE_H_

#include "IUiMode.h"
#include "../common/Vbos.h"

class UiBiomesMode final : public IUiMode {
 public:
  explicit UiBiomesMode(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue);

  void Render() override;
  void RenderPicking() override;
  data::TextId Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

  void BtnWind();

  void BtnSun();

  void BtnTime();

  void BtnPrecipitations();

  void BtnTemperature();

  void BtnClouds();

 protected:
  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiStaticSprite btn_wind_;
  UiStaticSprite btn_sun_;
  UiStaticSprite btn_time_;
  UiStaticSprite btn_precipitations_;
  UiStaticSprite btn_temperature_;
  UiStaticSprite btn_clouds_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kBiomesEditCloudIcon) -
      static_cast<int>(data::VboIdMain::kBiomesBiomesMode) + 1
      > ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIBIOMESMODE_H_
