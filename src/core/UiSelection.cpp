#include "UiSelection.h"

int UiSelection::gMaxPoints = 1000;

UiSelection::UiSelection(
    UiSharedResources& ui_shared_resources)
    : sp_circle_(data::VboIdMain::kBiomesTimeArea),
      ui_shared_resources_(ui_shared_resources),
      shader_("../shaders/Stipple.vert",
              "../shaders/Stipple.frag") {
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
}

void UiSelection::DeInit() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
}

void UiSelection::Render() {
  glBindVertexArray(vao_);
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(render_offset_));
  glDrawArrays(GL_LINE_STRIP, 0, vertex_num_);
}

void UiSelection::RenderSelectionCircle() {
  auto cursor_pos = ui_shared_resources_.global_glfw_callback_data_
                        .cursor_pos_tex_norm_;
  sp_circle_.SetTranslate(cursor_pos);
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  sp_circle_.Render();
}

void UiSelection::ResetBufferData() {
  vertex_num_ = 0;
}

void UiSelection::SetStartPos(glm::vec2 start_pos) {
  rectangle_start_pos_ = start_pos;
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
}

void UiSelection::SetLassoSelection(
    const std::vector<glm::vec2>& positions) {
  if (positions.size() + 1 > gMaxPoints) { // +1 to loop first/last
    ResetBufferData();
    throw "unable to draw polygon, gMaxPoints exceeded";
  }
  if (positions.size() < 3) {
    ResetBufferData(); // waiting for more points
    return;
  }
  auto polygon_copy = positions;
  std::vector<glm::vec3> data(positions.size() + 1);
  polygon_copy[0] = {
      positions[0].x * gWindowWidth / 2.0f,
      positions[0].y * gWindowHeight / 2.0f
  };
  data[0] = glm::vec3(positions[0].x, positions[0].y, 0.0f);
  for (int i = 1; i < positions.size(); ++i) {
    polygon_copy[i] = {
        positions[i].x * gWindowWidth / 2.0f,
        positions[i].y * gWindowHeight / 2.0f
    };
    float length = glm::length(polygon_copy[i] - polygon_copy[i - 1]);
    float prev_length = data[i - 1].z;
    data[i] = glm::vec3(positions[i].x, positions[i].y, length + prev_length);
  }
  // loop last/first
  int last_idx = positions.size() - 1;
  float length = glm::length(polygon_copy[0] - polygon_copy[last_idx]);
  float prev_length = data[last_idx].z;
  data[last_idx + 1] = glm::vec3(positions[0].x, positions[0].y, length + prev_length);

  UpdateRenderData(data, 2.0f);
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
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_num_ * sizeof(glm::vec3),
                  polygon.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  shader_.Bind();
  glUniform1f(1, stipple_width);
  glUseProgram(0); // unbind shader
}
