#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIFENCESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIFENCESMODE_H_

#include "IUiMode.h"
#include "../core/UiSlots.h"
#include "../common/Vbos.h"
#include "../common/ArbitraryGraph.h"

class UiFencesMode final : public IUiMode {
 public:
  UiFencesMode(UiSharedResources& ui_shared_resources);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

  void Remove();
  void BakePicket();
  void BakeChainLinked();
  void BakeWooden();

 protected:
  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiStaticSprite btn_bake_picket_;
  UiStaticSprite btn_bake_chain_linked_;
  UiStaticSprite btn_bake_wooden_;
  UiStaticSprite btn_remove_;

  ArbitraryGraph graphs_;
  UiSlots slots_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kFencesSlotsFlipSelectEdit) -
      static_cast<int>(data::VboIdMain::kFencesUiFencesMode) + 1
      > ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIFENCESMODE_H_
