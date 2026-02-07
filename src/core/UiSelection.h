#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UISELECTION_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UISELECTION_H_

#include <glad/glad.h>

#include <set>

#include "../ui/UiRenderData.h"
#include "../render/Shader.h"
#include "Ui.h"

class TerrainTraits;

enum class SelectionMode { kRectangle, kCircle, kLasso, kTweak };

class UiSelection {
 public:
  static int gMaxPoints;

  UiSelection(UiRenderData& render_data);

  ~UiSelection() { DeInit(); }

  void SetIdBounds(GLuint bound_min, GLuint bound_max) {
    bound_min_ = bound_min;
    bound_max_ = bound_max;
  }

  void Render();

  void RenderOnSurface(const Texture32F* surface);

  void Start(glm::vec2 cursor_pos, bool mod_ctrl, bool mod_shift);

  void Stop(glm::vec2 cursor_pos);

  std::set<GLuint> StopIntoSet(glm::vec2 cursor_pos);

  void Update(glm::vec2 mouse_pos);

  bool Scroll(float yoffset);

  /// should matter only on the beginning
  void SetMods(bool mod_ctrl, bool mod_shift) {
    mod_ctrl_ = mod_ctrl;
    mod_shift_ = mod_shift;
  }

  void SetMode(SelectionMode mode);

  /// use at menu mode switches, no check selection_mode_ == mode
  void SetModeForce(SelectionMode mode);

  void NextMode();

  [[nodiscard]] const Texture& GetMask() const noexcept {
    return selection_tex_surface_;
  }

  void SetMask(const Texture& mask);

 private:
  void Init();

  void InitCursors();

  void InitSelectionFbo();

  void DeInit();

  /// to update the buffer data (internal UpdateRenderData() call).
  /// use normalized cursor pos (-1;1 scale)
  void SelectRectangle(glm::vec2 end_pos);

  void SelectCircle(glm::vec2 mouse_pos);

  void SelectLasso(glm::vec2 mouse_pos);

  void SelectTweak();

  void RenderAreaLike();

  void RenderCircleLike();

  void RenderSelectionCircle();

  void ResetConfig();

  void ClearMask();

  void ClearSelectionFbo();

  void UpdateRenderData(const std::vector<glm::vec3>& polygon,
                        float stipple_width);

  void UpdateSurfaceSelection(float radius);

  void ApplySelection();

  std::set<GLuint> ApplySelectionIntoSet();

  UiSprite sp_circle_;
  UiRenderData& render_data_;
  Shader shader_;
  Shader shader_area_;
  Shader shader_draw_selection_;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;

  /// .comp shader + fbo
  GLuint selection_fbo_ = 0;
  Texture selection_tex_;
  UiSprite sp_selection_;

  Texture selection_tex_surface_;

  glm::vec2 mouse_check_point_ = glm::vec2(0.0f);
  bool start_is_end_ = true;

  GLuint vertex_num_ = 0;
  float circle_radius_ = 0.05f;

  bool mod_ctrl_ = false;
  bool mod_shift_ = false;

  double last_update_time_ = 0.0f;

  // parallel and we could merge it... and slightly destroy the readability
  glm::vec2 rectangle_start_pos_ = glm::vec2(0.0f);
  glm::vec2 render_offset_ = glm::vec2(0.0f);  /// circle follow the cursor

  std::vector<glm::vec2> lasso_data_;
  SelectionMode selection_mode_ = SelectionMode::kRectangle;
  bool selecting_ = false;

  GLuint bound_min_ = 0;
  GLuint bound_max_ = 0;

  GLFWcursor* csr_rectangle_ = nullptr;
  GLFWcursor* csr_circle_ = nullptr;
  GLFWcursor* csr_lasso_ = nullptr;
  GLFWcursor* csr_tweak_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UISELECTION_H_
