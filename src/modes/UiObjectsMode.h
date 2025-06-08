#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_

#include "IUiMode.h"
#include "../common/Vbos.h"

class UiObjectsMode final : public IUiMode {
 public:
  explicit UiObjectsMode(UiSharedResources& ui_shared_resources);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

  void BtnMavka();

  void BtnVodyaniy();

  void BtnChugaister();

  void BtnPedestal();

  void BtnCampfire();

  void BtnRoadSign();

  void BtnHuman();

 protected:
  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiStaticSprite btn_mavka_;
  UiStaticSprite btn_vodyaniy_;
  UiStaticSprite btn_chugaister_;
  UiStaticSprite btn_pedestal_;
  UiStaticSprite btn_campfire_;
  UiStaticSprite btn_road_sign_;
  UiStaticSprite btn_human_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kObjectsHuman) -
      static_cast<int>(data::VboIdMain::kObjectsUiObjectsMode) + 1
      > ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIOBJECTSMODE_H_
