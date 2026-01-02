#include "UiSelection.h"

#include <stb_image.h>

#include "../common/OpenGLUtility.h"
#include "../common/PickingFramebuffer.h"
#include "../modes/TerrainInstanceData.h"
#include "TileRenderer.h"

int UiSelection::gMaxPoints = 1000;

UiSelection::UiSelection(UiSharedResources& ui_shared_resources)
    : sp_circle_(data::VboIdMain::kSelectionCircle),
      ui_shared_resources_(ui_shared_resources),
      shader_("../shaders/Stipple.vert", "../shaders/Stipple.frag"),
      shader_area_("../shaders/SelectionArea.vert",
                   "../shaders/SelectionArea.frag"),
      shader_draw_selection_("../shaders/DrawSelection.comp"),
      sp_selection_(data::VboIdMain::kSelectionSprite),
      selection_tex_(gWindowWidth, gWindowHeight, GL_R8, GL_NEAREST,
                     GL_CLAMP_TO_EDGE),
      selection_tex_surface_(details::gTerrainSize, details::gTerrainSize,
                             GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE),
      mouse_check_point_(gWindowWidth / 2.0f, gWindowHeight / 2.0f) {
  Init();
}

void UiSelection::Render() {
  if (selection_mode_ == SelectionMode::kCircle) {
    RenderCircleLike();
    RenderSelectionCircle();
  } else if (selection_mode_ == SelectionMode::kRectangle ||
             selection_mode_ == SelectionMode::kLasso) {
    RenderAreaLike();
  }
}

void UiSelection::RenderOnSurface(const Texture32F* surface) {
  glm::vec3 color = glm::vec3(0.8f, 0.8f, 0.1f);
  ui_shared_resources_.gltf_context_.tile_renderer->terrain.RenderSelection(
      surface, selection_tex_surface_, color);
}

void UiSelection::Start(glm::vec2 cursor_pos, bool mod_ctrl, bool mod_shift) {
  SetMods(mod_ctrl, mod_shift);
  lasso_data_.clear();
  rectangle_start_pos_ = cursor_pos;
  start_is_end_ = true;
  if (!mod_ctrl_) {
    if (mod_shift_) {
      start_is_end_ = false;
    } else {  // no modifiers
      vertex_num_ = 0;
      ClearMask();
    }
  }
  selecting_ = true;
}

void UiSelection::Stop(glm::vec2 cursor_pos) {
  Update(cursor_pos);
  if (selection_mode_ == SelectionMode::kRectangle ||
      selection_mode_ == SelectionMode::kLasso) {
    shader_area_.Bind();
    glBindVertexArray(vao_);
    glUniform2fv(0, 1, glm::value_ptr(render_offset_));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, selection_fbo_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_num_ - 1);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }
  selecting_ = false;

  ApplySelection();
  ClearSelectionFbo();
  vertex_num_ = 0;
  mouse_check_point_ = ui_shared_resources_.gltf_context_.cursor_pos_;
  mouse_check_point_.y = gWindowHeight - mouse_check_point_.y;
}

void UiSelection::Update(glm::vec2 mouse_pos) {
  switch (selection_mode_) {
    case SelectionMode::kRectangle:
      SelectRectangle(mouse_pos);
      break;
    case SelectionMode::kCircle:
      SelectCircle(mouse_pos);
      break;
    case SelectionMode::kLasso:
      SelectLasso(mouse_pos);
      break;
    case SelectionMode::kTweak:
      SelectTweak();
      break;
  }
}

bool UiSelection::Scroll(float yoffset) {
  if (selecting_ && selection_mode_ == SelectionMode::kCircle) {
    float new_circle_radius = circle_radius_ + yoffset * 0.01f;
    circle_radius_ = std::clamp(new_circle_radius, 0.005f, 1.0f);
    SelectCircle(render_offset_);
  }
  return selecting_;
}

void UiSelection::SetMode(SelectionMode mode) {
  if (selecting_ || selection_mode_ == mode) {
    return;
  }
  SetModeForce(mode);
}

void UiSelection::SetModeForce(SelectionMode mode) {
  if (selecting_) {
    Stop(rectangle_start_pos_);
  }
  ResetConfig();
  selection_mode_ = mode;
  switch (selection_mode_) {
    case SelectionMode::kRectangle:
      glfwSetCursor(gWindow, csr_rectangle_);
      break;
    case SelectionMode::kCircle:
      glfwSetCursor(gWindow, csr_circle_);
      break;
    case SelectionMode::kLasso:
      glfwSetCursor(gWindow, csr_lasso_);
      break;
    case SelectionMode::kTweak:
      glfwSetCursor(gWindow, csr_tweak_);
      break;
  }
}

void UiSelection::NextMode() {
  SelectionMode next_mode;
  switch (selection_mode_) {
    case SelectionMode::kRectangle:
      next_mode = SelectionMode::kCircle;
      break;
    case SelectionMode::kCircle:
      next_mode = SelectionMode::kLasso;
      break;
    case SelectionMode::kLasso:
      next_mode = SelectionMode::kTweak;
      break;
    default:
      next_mode = SelectionMode::kRectangle;
  }
  SetModeForce(next_mode);
}

void UiSelection::SetMask(const Texture& mask) {
  glCopyImageSubData(mask.GetId(), GL_TEXTURE_2D, 0, 0, 0, 0,
                     selection_tex_surface_.GetId(), GL_TEXTURE_2D, 0, 0, 0, 0,
                     details::gTerrainSize, details::gTerrainSize, 1);
}

void UiSelection::Init() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, gMaxPoints * sizeof(glm::vec3), nullptr,
               GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  InitSelectionFbo();

  float res_factor =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  sp_selection_.SetExtraScale(res_factor);
  float scale_diff =
      2.0f / (sp_selection_.GetTopBorder() - sp_selection_.GetBottomBorder());
  sp_selection_.SetScale(scale_diff);

  ClearMask();
  InitCursors();
}

void UiSelection::InitCursors() {
  GLFWimage csr_image;
  csr_image.width = 32;
  csr_image.height = 32;
  unsigned char* csr_data;
  int width, height, channels;

  stbi_set_flip_vertically_on_load(false);

  csr_data =
      stbi_load("../assets/CursorRectangle.png", &width, &height, &channels, 0);
  csr_image.pixels = csr_data;
  csr_rectangle_ = glfwCreateCursor(&csr_image, 0, 0);

  csr_data =
      stbi_load("../assets/CursorCircle.png", &width, &height, &channels, 0);
  csr_image.pixels = csr_data;
  csr_circle_ = glfwCreateCursor(&csr_image, 0, 0);

  csr_data =
      stbi_load("../assets/CursorLasso.png", &width, &height, &channels, 0);
  csr_image.pixels = csr_data;
  csr_lasso_ = glfwCreateCursor(&csr_image, 0, 0);

  csr_data =
      stbi_load("../assets/CursorTweak.png", &width, &height, &channels, 0);
  csr_image.pixels = csr_data;
  csr_tweak_ = glfwCreateCursor(&csr_image, 0, 0);

  stbi_set_flip_vertically_on_load(true);
}

void UiSelection::InitSelectionFbo() {
  glGenFramebuffers(1, &selection_fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, selection_fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         selection_tex_.GetId(), 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("framebuffer is not complete");
  }
  GLenum bufs[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, bufs);
  ClearSelectionFbo();
}

void UiSelection::DeInit() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
  glDeleteFramebuffers(1, &selection_fbo_);
  glfwDestroyCursor(csr_rectangle_);
  glfwDestroyCursor(csr_circle_);
  glfwDestroyCursor(csr_lasso_);
  glfwDestroyCursor(csr_tweak_);
}

void UiSelection::SelectRectangle(glm::vec2 end_pos) {
  if (glm::length(rectangle_start_pos_ - end_pos) < 0.01f) {
    vertex_num_ = 0;
    return;
  }
  float width = std::abs(end_pos.x - rectangle_start_pos_.x);
  float height = std::abs(rectangle_start_pos_.y - end_pos.y);
  std::vector<glm::vec3> data = {
      {rectangle_start_pos_.x, rectangle_start_pos_.y, 0.0f},
      {end_pos.x, rectangle_start_pos_.y, width * gWindowWidth / 2.0f},
      {end_pos.x, end_pos.y,
       height * gWindowHeight / 2.0f + width * gWindowWidth / 2.0f},
      {rectangle_start_pos_.x, end_pos.y,
       width * gWindowWidth + height * gWindowHeight / 2.0f},
      {rectangle_start_pos_.x, rectangle_start_pos_.y,
       width * gWindowWidth + height * gWindowHeight},
  };
  UpdateRenderData(data, 8.0f);
}

void UiSelection::SelectCircle(glm::vec2 mouse_pos) {
  render_offset_ = mouse_pos;
  float res_factor =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  // segments_num = points_num - 1, but +1 to loop last
  int segments =
      static_cast<int>(circle_radius_ * static_cast<float>(gMaxPoints));
  std::vector<glm::vec3> data(segments);
  for (int i = 0; i < segments; i++) {
    float theta = 2.0f * M_PI * i / segments;
    data[i] = {circle_radius_ * cos(theta) / res_factor,
               circle_radius_ * sin(theta), i};
  }
  UpdateRenderData(data, 2.0f);
  UpdateSurfaceSelection(circle_radius_);
  ApplySelection();
}

void UiSelection::SelectLasso(glm::vec2 mouse_pos) {
  lasso_data_.push_back(mouse_pos);
  if (lasso_data_.size() + 1 > gMaxPoints || lasso_data_.size() < 3) {
    vertex_num_ = 0;
    return;
  }
  auto polygon_copy = lasso_data_;
  std::vector<glm::vec3> data(lasso_data_.size() + 1);
  polygon_copy[0] = {lasso_data_[0].x * gWindowWidth / 2.0f,
                     lasso_data_[0].y * gWindowHeight / 2.0f};
  data[0] = glm::vec3(lasso_data_[0].x, lasso_data_[0].y, 0.0f);
  for (int i = 1; i < lasso_data_.size(); ++i) {
    polygon_copy[i] = {lasso_data_[i].x * gWindowWidth / 2.0f,
                       lasso_data_[i].y * gWindowHeight / 2.0f};
    float length = glm::length(polygon_copy[i] - polygon_copy[i - 1]);
    float prev_length = data[i - 1].z;
    data[i] =
        glm::vec3(lasso_data_[i].x, lasso_data_[i].y, length + prev_length);
  }
  // loop last/first
  int last_idx = lasso_data_.size() - 1;
  float length = glm::length(polygon_copy[0] - polygon_copy[last_idx]);
  float prev_length = data[last_idx].z;
  data[last_idx + 1] =
      glm::vec3(lasso_data_[0].x, lasso_data_[0].y, length + prev_length);

  UpdateRenderData(data, 2.0f);
}

void UiSelection::SelectTweak() {
  UpdateSurfaceSelection(0.001f);  // min radius -> selects a single vertex
  ApplySelection();
}

void UiSelection::RenderAreaLike() {
  if (vertex_num_ == 0) {
    return;
  }
  glBindVertexArray(vao_);
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(render_offset_));
  glDrawArrays(GL_LINE_STRIP, 0, vertex_num_);

  shader_area_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(render_offset_));
  glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_num_ - 1);
}

void UiSelection::RenderCircleLike() {
  glBindVertexArray(vao_);
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(render_offset_));
  glDrawArrays(GL_LINE_STRIP, 0, vertex_num_);
}

void UiSelection::RenderSelectionCircle() {
  auto cursor_pos = ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_;
  sp_circle_.SetTranslate(cursor_pos);
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.shader_sp_.Bind();
  sp_circle_.Render();
}

void UiSelection::ResetConfig() {
  circle_radius_ = 0.05f;
  render_offset_ = glm::vec2(0.0f);
}

void UiSelection::ClearMask() {
  GLuint black = 0;
  glClearTexImage(selection_tex_surface_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                  &black);
}

void UiSelection::ClearSelectionFbo() {
  GLuint black = 0;
  glClearTexImage(selection_tex_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, &black);
}

void UiSelection::UpdateRenderData(const std::vector<glm::vec3>& polygon,
                                   float stipple_width) {
  vertex_num_ = polygon.size();
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_num_ * sizeof(glm::vec3),
                  polygon.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  shader_.Bind();
  glUniform1f(1, stipple_width);
}

// only for tweak and circle
void UiSelection::UpdateSurfaceSelection(float radius) {
  glm::vec2 mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_;
  mouse_pos.y = gWindowHeight - mouse_pos.y;
  shader_draw_selection_.Bind();
  const auto& fbo_tex =
      ui_shared_resources_.gltf_context_.picking_fbo->GetTex();
  utility::BindImageTexture(0, selection_tex_, GL_WRITE_ONLY);
  utility::BindImageTexture(1, fbo_tex, GL_READ_ONLY);
  glUniform1f(2, radius);
  if (start_is_end_) {
    glUniform2fv(0, 1, glm::value_ptr(mouse_pos));
    glUniform2fv(1, 1, glm::value_ptr(mouse_pos));
    start_is_end_ = false;
  } else {
    glUniform2fv(0, 1, glm::value_ptr(mouse_check_point_));
    glUniform2fv(1, 1, glm::value_ptr(mouse_pos));
  }
  GLuint workGroupSizeX = (gWindowWidth + 15) / 16;
  GLuint workGroupSizeY = (gWindowHeight + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  mouse_check_point_ = mouse_pos;
  utility::UnBindImageTexture(0, selection_tex_, GL_WRITE_ONLY);
  utility::UnBindImageTexture(1, fbo_tex, GL_READ_ONLY);
}

/**
 * GPU version invalid, need shader to accumulate selection to ssbo,
 * then another. otherwise race-cond (or not, but have some garbage
 * outside the selection - idk how to fix) see shaders/SelectTerrain.comp
 */
void UiSelection::ApplySelection() {
  double prev_time_ = last_update_time_;
  double cur_time = glfwGetTime();
  auto time_diff = cur_time - prev_time_;
  if (time_diff < 0.1f) {
    return;
  }
  last_update_time_ = cur_time;
  const auto& fbo_tex =
      ui_shared_resources_.gltf_context_.picking_fbo->GetTex();
  int buffer_size = gWindowWidth * gWindowHeight;

  std::vector<uint8_t> selection_data(buffer_size);
  glGetTextureImage(selection_tex_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                    buffer_size * sizeof(uint8_t), selection_data.data());

  std::vector<GLuint> id_data(buffer_size);
  glGetTextureImage(fbo_tex.GetId(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT,
                    buffer_size * sizeof(GLuint), id_data.data());

  int surface_size = details::gTerrainSize * details::gTerrainSize;
  std::vector<uint8_t> result(surface_size);
  glGetTextureImage(selection_tex_surface_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                    surface_size * sizeof(uint8_t), result.data());

  for (int i = 0; i < buffer_size; ++i) {
    if (!selection_data[i]) {
      continue;
    }
    GLuint id = id_data[i];
    if (id < bound_min_ || id > bound_max_) {
      continue;
    }
    id -= bound_min_;
    // 1: add (shift) connect last to new,
    // 1: add (ctrl) no connect last to new,
    // 0: erase (ctrl + shift)
    float mask_factor = 1.0f;
    if (mod_ctrl_ && mod_shift_) {
      mask_factor = 0.0f;
    }
    auto y = static_cast<int>(id & 1023);
    auto x = static_cast<int>(id >> 10);
    result[y * 1024 + x] = 255 * mask_factor;
  }
  glTextureSubImage2D(selection_tex_surface_.GetId(), 0, 0, 0,
                      details::gTerrainSize, details::gTerrainSize, GL_RED,
                      GL_UNSIGNED_BYTE, result.data());
}

// TODO: these two are pretty ugly

std::set<GLuint> UiSelection::ApplySelectionIntoSet() {
  const auto& fbo_tex =
      ui_shared_resources_.gltf_context_.picking_fbo->GetTex();
  int buffer_size = gWindowWidth * gWindowHeight;

  std::vector<uint8_t> selection_data(buffer_size);
  glGetTextureImage(selection_tex_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE,
                    buffer_size * sizeof(uint8_t), selection_data.data());

  std::vector<GLuint> id_data(buffer_size);
  glGetTextureImage(fbo_tex.GetId(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT,
                    buffer_size * sizeof(GLuint), id_data.data());

  std::set<GLuint> selected_ids;
  for (int i = 0; i < buffer_size; ++i) {
    if (!selection_data[i]) {
      continue;
    }
    GLuint id = id_data[i];
    if (id < bound_min_ || id > bound_max_) {
      continue;
    }
    selected_ids.insert(id);
  }
  return selected_ids;
}

std::set<GLuint> UiSelection::StopIntoSet(glm::vec2 cursor_pos) {
  Update(cursor_pos);
  if (selection_mode_ == SelectionMode::kRectangle ||
      selection_mode_ == SelectionMode::kLasso) {
    shader_area_.Bind();
    glBindVertexArray(vao_);
    glUniform2fv(0, 1, glm::value_ptr(render_offset_));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, selection_fbo_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_num_ - 1);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }
  selecting_ = false;

  auto selected_ids = ApplySelectionIntoSet();
  ClearSelectionFbo();
  vertex_num_ = 0;
  mouse_check_point_ = ui_shared_resources_.gltf_context_.cursor_pos_;
  mouse_check_point_.y = gWindowHeight - mouse_check_point_.y;
  return selected_ids;
}
