#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_ROADSMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_ROADSMODE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "IEditMode.h"
#include "../core/Ui.h"

void RoadsModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset);

void RoadsModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods);

void RoadsModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

class RoadsMode final : public IEditMode {
 public:
  RoadsMode(SharedResources& shared_resources,
            const TextRenderer& text_renderer);

  void Render() override;
  void RenderPicking() override;
  int Hover(std::uint32_t global_id) override;

  void Create(GLuint id);

  void Remove();

  void BakeAsphalt();
  void BakeGravel();
  void BakeSoil();

  void BindCallbacks() override;

 protected:
  friend void RoadsModeScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  friend void RoadsModeMouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  friend void RoadsModeKeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiStaticSprite btn_bake_asphalt_;
  UiStaticSprite btn_bake_gravel_;
  UiStaticSprite btn_bake_soil_;
  UiStaticSprite btn_create_;
  UiStaticSprite btn_remove_;
  UiSlots slots_;

  std::size_t temp_size_{0}; // TODO: remove

  using PointDataType = std::vector<Point>;
  PointDataType points_data_test_{};
  int edit_mode_selected_sample_id_test_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_ROADSMODE_H_
