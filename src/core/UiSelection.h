#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UISELECTION_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UISELECTION_H_

#include <glad/glad.h>

#include "Ui.h"
#include "../modes/UiSharedResources.h"
#include "../common/Shader.h"

class UiSelection {
 public:
  static int gMaxPoints;

  UiSelection(UiSharedResources& ui_shared_resources);

  ~UiSelection() {
    DeInit();
  }

  void Render();

  void RenderSelectionCircle();

  void ResetBufferData();

  void SetStartPos(glm::vec2 start_pos);

  /// to update the buffer data (internal UpdateRenderData() call).
  /// use normalized cursor pos (-1;1 scale)
  void SetRectangleSelection(glm::vec2 end_pos);

  void SetCircleSelection(glm::vec2 mouse_pos);

  void SetLassoSelection(const std::vector<glm::vec2>& positions);

  void SetCircleRadius(float radius);

  void StepCircleRadius(float step);

 private:
  void Init();

  void DeInit();

  void UpdateRenderData(
      const std::vector<glm::vec3>& polygon, float stipple_width);

  UiDynamicSprite sp_circle_;
  UiSharedResources& ui_shared_resources_;
  Shader shader_;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;

  GLuint vertex_num_ = 0;
  float circle_radius_ = 0.1f;

  // parallel and we could merge it... and slightly destroy the readability
  glm::vec2 rectangle_start_pos_ = glm::vec2(0.0f);
  glm::vec2 render_offset_ = glm::vec2(0.0f); /// circle follow the cursor
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UISELECTION_H_
