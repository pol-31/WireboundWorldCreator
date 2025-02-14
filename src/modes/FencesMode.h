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

class FencesMode final : public IEditMode {
 public:
  FencesMode(SharedResources& shared_resources,
             const TextRenderer& text_renderer);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void Create(GLuint id);

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
  UiSlots slots_;

  std::size_t temp_size_{0}; // TODO: remove

  using PointDataType = std::vector<Point>;
  PointDataType points_data_test_{};
  int edit_mode_selected_sample_id_test_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_
