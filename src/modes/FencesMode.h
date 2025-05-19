#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"

void FencesModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void FencesModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void FencesModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

/* btn_slot_remove, btn_slot_add
 * tap on map (nothing pressed) ->
 * - <mode points> - select one points (to remove or...)
 * - <mode edges> - select two points (to remove or...)
 * tap on map (edit was pressed) ->
 * - <mode points> - add point if not exists
 * - <mode edges> - select one, select two, create
 * two states: points/edges, edit/select
 * */
/// so total we have 2 states, remove button, two buttons for each slot

class FencesMode final : public IEditMode {
 public:
  FencesMode(SharedResources& shared_resources,
             const TextRenderer& text_renderer);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void Remove();

  void BakePicket();
  void BakeChainLinked();
  void BakeWooden();

  void BindCallbacks() override;

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
  UiStaticSprite btn_create_;
  UiStaticSprite btn_remove_;

  UiStaticSprite btn_edge_mode_;
  UiStaticSprite btn_press_mode_;

  //TODO: refactor (move to ArbitraryGraph graphs_)
  UiSlots slots_;
  std::size_t temp_size_{0};
  int edit_mode_selected_sample_id_test_{0};

  ArbitraryGraph graphs_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_
