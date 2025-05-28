#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"

#include "../common/ArbitraryGraph.h"

void FencesModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void FencesModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void FencesModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class FencesMode final : public IEditMode {
 public:
  FencesMode(SharedResources& shared_resources,
             const TextRenderer& text_renderer);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void BindCallbacks() override;

  //TODO:
  void Remove();
  void BakePicket();
  void BakeChainLinked();
  void BakeWooden();

 protected:
  friend void FencesModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void FencesModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void FencesModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiStaticSprite btn_bake_picket_;
  UiStaticSprite btn_bake_chain_linked_;
  UiStaticSprite btn_bake_wooden_;
  UiStaticSprite btn_remove_;

  ArbitraryGraph graphs_;
  UiSlots slots_;

  UiEventHandler<
      static_cast<int>(vbos::VboIdMain::kFencesSlotsFlipSelectEdit) -
      static_cast<int>(vbos::VboIdMain::kFencesFencesMode) + 1
      > ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_
