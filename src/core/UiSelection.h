#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UISELECTION_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UISELECTION_H_

#include <glad/glad.h>

#include "Ui.h"
#include "../modes/UiSharedResources.h"
#include "../common/Shader.h"

class TerrainInstanceData;

enum class SelectionMode {
  kRectangle,
  kCircle,
  kLasso,
  kTweak
};

class UiSelection {
 public:
  static int gMaxPoints;

  UiSelection(UiSharedResources& ui_shared_resources);

  ~UiSelection() {
    DeInit();
  }

  void SetIdBounds(GLuint bound_min, GLuint bound_max);

  void RenderAreaLike();

  void RenderCircleLike();

  void RenderSelectionCircle();

  void ResetBufferData();

  // at mode switching (e.g. reset circle selection radius)
  void ResetSelection();

  /// to update the buffer data (internal UpdateRenderData() call).
  /// use normalized cursor pos (-1;1 scale)
  void SetRectangleSelection(glm::vec2 end_pos);

  void SetCircleSelection(glm::vec2 mouse_pos);

  void SetLassoSelection(glm::vec2 mouse_pos);

  void SetTweakSelection();

  void SetCircleRadius(float radius);

  void StepCircleRadius(float step);

 void Release();

 void ClearSelection();

 void SelectPoints();

 /// should matter only on the beginning
 void SetMods(bool mod_ctrl, bool mod_shift) {
   mod_ctrl_ = mod_ctrl;
   mod_shift_ = mod_shift;
 }

 void SetSelectionMode(SelectionMode mode);

 /// use at mode switches, no check selection_mode_ == mode
 void SetSelectionModeForce(SelectionMode mode);

 void Render();

 void RenderSurfaceSelection(const Texture32F* surface);

 void UpdateSelection(glm::vec2 mouse_pos);

 bool ScrollSelection(float yoffset);

 void StartSelecting(glm::vec2 cursor_pos, bool mod_ctrl, bool mod_shift);

 void StopSelecting(glm::vec2 cursor_pos);

 void NextSelectionMode();

 [[nodiscard]] const Texture& GetSelectionMask() const noexcept {
   return selection_tex_surface_;
 }

 void SetSelectionMask(const Texture& mask);

 private:
  void Init();

  void InitSelectionFbo();

  void InitCursors();

  void DeInit();

  void UpdateRenderData(
      const std::vector<glm::vec3>& polygon, float stipple_width);

  void UpdateSurfaceSelection(float radius);

  void ApplySelection();

  void ClearSelectionFbo();

  UiDynamicSprite sp_circle_;
  UiSharedResources& ui_shared_resources_;
  Shader shader_;
  Shader shader_area_;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;

  /// .comp shader + fbo
  GLuint selection_fbo_ = 0;
  Texture selection_tex_;
  UiDynamicSprite sp_selection_;

  Texture selection_tex_surface_;
  Shader shader_select_;

  glm::vec2 mouse_check_point_;
  bool start_is_end_ = true;
  bool pressed_ = false;

  GLuint vertex_num_ = 0;
  float circle_radius_ = 0.05f;

  bool mod_ctrl_ = false;
  bool mod_shift_ = false;

  // parallel and we could merge it... and slightly destroy the readability
  glm::vec2 rectangle_start_pos_ = glm::vec2(0.0f);
  glm::vec2 render_offset_ = glm::vec2(0.0f); /// circle follow the cursor

  std::vector<glm::vec2> lasso_data_;
  SelectionMode selection_mode_ = SelectionMode::kRectangle;
  bool selecting_ = false;

  GLuint bound_min_ = 0;
  GLuint bound_max_ = 0;

  GLFWcursor* csr_rectangle_;
  GLFWcursor* csr_circle_;
  GLFWcursor* csr_lasso_;
  GLFWcursor* csr_tweak_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UISELECTION_H_
