#include "UiSelection.h"

#include "../common/OpenGLUtility.h"
#include "../common/PickingFramebuffer.h"
#include "../modes/TerrainInstanceData.h"
#include "TileRenderer.h"

int UiSelection::gMaxPoints = 1000;

UiSelection::UiSelection(
    UiSharedResources& ui_shared_resources)
    : sp_circle_(data::VboIdMain::kBiomesTimeArea),
      ui_shared_resources_(ui_shared_resources),
      shader_("../shaders/Stipple.vert",
              "../shaders/Stipple.frag"),
      shader_area_("../shaders/SelectionArea.vert",
                   "../shaders/SelectionArea.frag"),
      shader_select_("../shaders/generate_shaders/SelectTerrain.comp"),
      sp_selection_(data::VboIdMain::kSpare5),
      selection_tex_(gWindowWidth, gWindowHeight, GL_R8),
      selection_tex_surface_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      mouse_check_point_(gWindowWidth / 2.0f, gWindowHeight / 2.0f) {
  Init();
}

void UiSelection::Init() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, gMaxPoints * sizeof(glm::vec3),
               nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  InitSelectionFbo();

  float res_factor = static_cast<float>(gWindowWidth)
                     / static_cast<float>(gWindowHeight);
  sp_selection_.SetExtraScale(res_factor);
  float scale_diff = 2.0f / (sp_selection_.GetTopBorder()
                             - sp_selection_.GetBottomBorder());
  sp_selection_.SetScale(scale_diff);

  ClearSelection();
}

void UiSelection::InitSelectionFbo() {
  glGenFramebuffers(1, &selection_fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, selection_fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         selection_tex_.GetId(), 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("framebuffer is not complete");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  ClearSelectionFbo();
}

void UiSelection::ClearSelectionFbo() {
  std::cout << "clear selection fbo" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, selection_fbo_);
  GLuint clear_id = 0;
  glClearTexImage(selection_tex_.GetId(), 0, GL_RED,
                  GL_UNSIGNED_BYTE, &clear_id);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UiSelection::DeInit() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
  glDeleteFramebuffers(1, &selection_fbo_);
}

void UiSelection::RenderAreaLike(const Texture32F* surface) {
  glBindVertexArray(vao_);
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(render_offset_));
  glDrawArrays(GL_LINE_STRIP, 0, vertex_num_);
  shader_area_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(render_offset_));
  glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_num_);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, selection_fbo_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_num_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  RenderSurfaceSelection(surface);
}

void UiSelection::RenderCircleLike(const Texture32F* surface) {
  glBindVertexArray(vao_);
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(render_offset_));
  glDrawArrays(GL_LINE_STRIP, 0, vertex_num_);
  RenderSurfaceSelection(surface);
}

void UiSelection::RenderSurfaceSelection(const Texture32F* surface) {
  glm::vec3 color = glm::vec3(0.8f, 0.8f, 0.1f);
  color = glm::vec3(0.2f, 0.9f, 0.8f);
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer
      ->terrain.RenderSelection(surface, selection_tex_surface_, color);
}

void UiSelection::RenderSelectionCircle() {
  auto cursor_pos = ui_shared_resources_.global_glfw_callback_data_
                        .cursor_pos_tex_norm_;
  sp_circle_.SetTranslate(cursor_pos);
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  sp_circle_.Render();
}

void UiSelection::ResetBufferData() {
  std::cout << "reset buffer data" << std::endl;
  vertex_num_ = 0;
}

void UiSelection::ResetSelection() {
  std::cout << "reset selection" << std::endl;
  circle_radius_ = 0.05f;
  render_offset_ = glm::vec2(0.0f);
}

void UiSelection::SetRectangleSelection(glm::vec2 end_pos) {
  if (glm::length(rectangle_start_pos_ - end_pos) < 0.01f) {
    ResetBufferData();
    return;
  }
  float width = std::abs(end_pos.x - rectangle_start_pos_.x);
  float height = std::abs(rectangle_start_pos_.y - end_pos.y);
  std::vector<glm::vec3> data = {
      {rectangle_start_pos_.x, rectangle_start_pos_.y, 0.0f},
      {end_pos.x, rectangle_start_pos_.y,
       width * gWindowWidth / 2.0f},
      {end_pos.x, end_pos.y,
       height * gWindowHeight / 2.0f + width * gWindowWidth / 2.0f},
      {rectangle_start_pos_.x, end_pos.y,
       width * gWindowWidth + height * gWindowHeight / 2.0f},
      {rectangle_start_pos_.x, rectangle_start_pos_.y,
       width * gWindowWidth + height * gWindowHeight},
  };
  UpdateRenderData(data, 8.0f);
}

void UiSelection::SetCircleSelection(glm::vec2 mouse_pos) {
  render_offset_ = mouse_pos;
  float res_factor = static_cast<float>(gWindowWidth)
                     / static_cast<float>(gWindowHeight);
  // segments_num = points_num - 1, but +1 to loop last
  int segments = static_cast<int>(circle_radius_ * static_cast<float>(gMaxPoints));
  std::vector<glm::vec3> data(segments);
  for (int i = 0; i < segments; i++) {
    float theta = 2.0f * M_PI * i / segments;
    data[i] = {circle_radius_ * cos(theta) / res_factor,
               circle_radius_ * sin(theta),
               i};
  }
  UpdateRenderData(data, 2.0f);
  UpdateSurfaceSelection(circle_radius_);
  ApplySelection();
}

void UiSelection::SetLassoSelection(glm::vec2 mouse_pos) {
  lasso_data_.push_back(mouse_pos);
  if (lasso_data_.size() + 1 > gMaxPoints) { // +1 to loop first/last
    ResetBufferData();
    throw "unable to draw polygon, gMaxPoints exceeded";
  }
  if (lasso_data_.size() < 3) {
    ResetBufferData(); // waiting for more points
    return;
  }
  auto polygon_copy = lasso_data_;
  std::vector<glm::vec3> data(lasso_data_.size() + 1);
  polygon_copy[0] = {
      lasso_data_[0].x * gWindowWidth / 2.0f,
      lasso_data_[0].y * gWindowHeight / 2.0f
  };
  data[0] = glm::vec3(lasso_data_[0].x, lasso_data_[0].y, 0.0f);
  for (int i = 1; i < lasso_data_.size(); ++i) {
    polygon_copy[i] = {
        lasso_data_[i].x * gWindowWidth / 2.0f,
        lasso_data_[i].y * gWindowHeight / 2.0f
    };
    float length = glm::length(polygon_copy[i] - polygon_copy[i - 1]);
    float prev_length = data[i - 1].z;
    data[i] = glm::vec3(lasso_data_[i].x, lasso_data_[i].y, length + prev_length);
  }
  // loop last/first
  int last_idx = lasso_data_.size() - 1;
  float length = glm::length(polygon_copy[0] - polygon_copy[last_idx]);
  float prev_length = data[last_idx].z;
  data[last_idx + 1] = glm::vec3(lasso_data_[0].x, lasso_data_[0].y, length + prev_length);

  UpdateRenderData(data, 2.0f);
}

void UiSelection::SetTweakSelection() {
  UpdateSurfaceSelection(0.001f); // min radius -> selects a single vertex
  ApplySelection();
}

void UiSelection::SetCircleRadius(float radius) {
  circle_radius_ = std::clamp(radius, 0.005f, 1.0f);
  SetCircleSelection(render_offset_);
}

void UiSelection::StepCircleRadius(float step) {
  SetCircleRadius(circle_radius_ + step);
}

void UiSelection::UpdateRenderData(
    const std::vector<glm::vec3>& polygon, float stipple_width) {
  vertex_num_ = polygon.size();
//  std::cout << polygon[2].x << ' ' << vertex_num_ << std::endl;
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_num_ * sizeof(glm::vec3),
                  polygon.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  shader_.Bind();
  glUniform1f(1, stipple_width);
  glUseProgram(0); // unbind shader
}

// only for tweak and circle
void UiSelection::UpdateSurfaceSelection(float radius) {
  if (!pressed_) {
    return;
  }
  glm::vec2 mouse_pos = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_;
  mouse_pos.y = gWindowHeight - mouse_pos.y;
  ui_shared_resources_.shader_terrain_selection_.Bind();
  const auto& fbo_tex = ui_shared_resources_.global_glfw_callback_data_
                            .picking_fbo->GetTex();
  utility::BindImageTexture(0, selection_tex_, GL_WRITE_ONLY);
  utility::BindImageTexture(1, fbo_tex, GL_READ_ONLY);
  glm::uvec2 point{};
  glUniform1f(2, radius);
  if (start_is_end_) {
    glUniform2fv(0, 1, glm::value_ptr(mouse_pos));
    glUniform2fv(1, 1, glm::value_ptr(mouse_pos));
    start_is_end_ = false;
  } else {
    glUniform2fv(0, 1, glm::value_ptr(mouse_check_point_));
    glUniform2fv(1, 1, glm::value_ptr(mouse_pos));
  }
  GLuint workGroupSizeX = (gWindowWidth  + 15) / 16;
  GLuint workGroupSizeY = (gWindowHeight + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  mouse_check_point_ = mouse_pos;
  utility::UnBindImageTexture(0, selection_tex_, GL_WRITE_ONLY);
  utility::UnBindImageTexture(1, fbo_tex, GL_READ_ONLY);
}

void UiSelection::Render(const Texture32F* surface) {
  if (!surface) {
    return;
  }
  switch (selection_mode_) {
    case SelectionMode::kCircle:
      RenderCircleLike(surface);
      RenderSelectionCircle();
      break;
    case SelectionMode::kTweak:
      RenderSurfaceSelection(surface);
      break;
    default:
      RenderAreaLike(surface);
  }
}

void UiSelection::UpdateSelection(glm::vec2 mouse_pos) {
  switch (selection_mode_) {
    case SelectionMode::kRectangle:
      SetRectangleSelection(mouse_pos);
      break;
    case SelectionMode::kCircle:
      SetCircleSelection(mouse_pos);
      break;
    case SelectionMode::kLasso:
      SetLassoSelection(mouse_pos);
      break;
    case SelectionMode::kTweak:
      SetTweakSelection();
      break;
  }
}

bool UiSelection::ScrollSelection(float yoffset) {
  if (selecting_ && selection_mode_ == SelectionMode::kCircle) {
    StepCircleRadius(yoffset * 0.01f);
  }
  return selecting_;
}

void UiSelection::StartSelecting(
    glm::vec2 mouse_pos, bool mod_ctrl, bool mod_shift) {
  SetMods(mod_ctrl, mod_shift);
  lasso_data_.clear();
  rectangle_start_pos_ = mouse_pos;
  pressed_ = true;
  start_is_end_ = true;
  if (!mod_ctrl_ && mod_shift_) {
    start_is_end_ = false;
  } else if (!mod_ctrl_ && !mod_shift_) { // mod_ctrl_ or no mods at all
    ResetBufferData();
    ClearSelection();
  }
  selecting_ = true;
}

const Texture* UiSelection::StopSelecting(glm::vec2 cursor_pos) {
  UpdateSelection(cursor_pos);
  selecting_ = false;
  return SelectPoints();
}

void UiSelection::Release() {
  pressed_ = false;
}

void UiSelection::ClearSelection() {
  std::cout << "clear selection" << std::endl;
  GLuint black = 0;
  glClearTexImage(selection_tex_surface_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, &black);
}

void UiSelection::ApplySelection() {
  shader_select_.Bind();
  const auto& fbo_tex = ui_shared_resources_.global_glfw_callback_data_.picking_fbo->GetTex();
  utility::BindImageTexture(0, fbo_tex, GL_READ_ONLY);
  utility::BindImageTexture(1, selection_tex_, GL_READ_ONLY);
  utility::BindImageTexture(2, selection_tex_surface_, GL_WRITE_ONLY);
  glUniform1ui(0, details::kIdOffsetWater);
  // 1: add (shift) connect last to new,
  // 1: add (ctrl) no connect last to new,
  // 0: erase (ctrl + shift)
  float mask_factor = 1.0f;
  if (mod_ctrl_ && mod_shift_) {
    mask_factor = 0.0f;
  }
  glUniform1f(1, mask_factor);
  GLuint workGroupSizeX = (gWindowWidth  + 15) / 16;
  GLuint workGroupSizeY = (gWindowHeight + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  utility::UnBindImageTexture(0, fbo_tex, GL_READ_ONLY);
  utility::UnBindImageTexture(1, selection_tex_, GL_READ_ONLY);
  utility::UnBindImageTexture(2, selection_tex_surface_, GL_WRITE_ONLY);
}

const Texture* UiSelection::SelectPoints() {
  ApplySelection();
  ClearSelectionFbo();
  ResetBufferData();
  mouse_check_point_ =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_;
  mouse_check_point_.y = gWindowHeight - mouse_check_point_.y;
  return &selection_tex_surface_;
}

void UiSelection::SetSelectionMode(SelectionMode mode) {
  if (selection_mode_ != mode) {
    ResetSelection();
    selection_mode_ = mode;
  }
}
