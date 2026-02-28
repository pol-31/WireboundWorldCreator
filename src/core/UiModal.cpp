#include "UiModal.h"

#include "../common/PickingFramebuffer.h"
#include "../modes/IUiMode.h"

UiConfirmation::UiConfirmation(UiRenderData& render_data,
                               TextRenderer& text_renderer)
    : render_data_(render_data),
      sp_background_(data::UiId::kConfirmationDesk),
      sp_accept_(data::UiId::kConfirmationAccept),
      sp_decline_(data::UiId::kConfirmationDecline),
      text_(text_renderer, data::UiId::kConfirmationText),
      hierarchy_(&sp_background_, {&sp_accept_, &sp_decline_, &text_}) {}

void UiConfirmation::Render() {
  if (!active_) {
    return;
  }
  glBindVertexArray(render_data_.vao_ui_);
  render_data_.shader_sp_.Bind();
  render_data_.tex_ui_.BindSampler(0);
  sp_background_.Render();
  sp_accept_.Render();
  sp_decline_.Render();
  text_.Render();
}

void UiConfirmation::RenderPicking() {
  if (!active_) {
    return;
  }
  render_data_.shader_sp_picking_.Bind();
  sp_background_.RenderPicking();
  sp_accept_.RenderPicking();
  sp_decline_.RenderPicking();
  text_.RenderPicking();
}

void UiConfirmation::Show(data::TextId text_id,
                          std::function<void()>&& callable) {
  text_.SetText(text_id);
  callable_ = std::move(callable);
  active_ = true;
  glfwSetScrollCallback(gWindow, nullptr);
  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
}

void UiConfirmation::Hide() {
  active_ = false;
  (*render_data_.glfw_context_.cur_mode)->Setup();
}

void UiConfirmation::KeyCallback(GLFWwindow* window, int key, int scancode,
                                 int action, int mods) {
  if (action == GLFW_RELEASE) {
    return;
  }
  auto ui = GetGlfwContext(window)->ui_confirmation;
  if (key == GLFW_KEY_ENTER) {
    ui->callable_();
  } else if (key == GLFW_KEY_ESCAPE) {
    ui->Hide();
  }
}

void UiConfirmation::MouseButtonCallback(GLFWwindow* window, int button,
                                         int action, int mods) {
  auto glfw_context = GetGlfwContext(window);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  auto ui = glfw_context->ui_confirmation;
  if (action != GLFW_PRESS || pressed_id == ui->sp_background_.GetId()) {
    return;
  }
  if (pressed_id == ui->sp_accept_.GetId()) {
    ui->callable_();
  }
  ui->Hide();
}

UiWarning::UiWarning(UiRenderData& render_data, TextRenderer& text_renderer)
    : render_data_(render_data),
      sp_background_(data::UiId::kCautionDesk),
      text_(text_renderer, data::UiId::kCautionText),
      hierarchy_(&sp_background_, {&text_}) {}

void UiWarning::Render() {
  if (!active_) {
    return;
  }
  glBindVertexArray(render_data_.vao_ui_);
  render_data_.shader_sp_.Bind();
  render_data_.tex_ui_.BindSampler(0);
  sp_background_.Render();
  text_.Render();
}

void UiWarning::RenderPicking() {
  if (!active_) {
    return;
  }
  render_data_.shader_sp_picking_.Bind();
  sp_background_.RenderPicking();
  text_.RenderPicking();
}

void UiWarning::Show(data::TextId text_id) {
  text_.SetText(text_id);
  active_ = true;
  glfwSetScrollCallback(gWindow, nullptr);
  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
}

void UiWarning::Hide() {
  active_ = false;
  (*render_data_.glfw_context_.cur_mode)->Setup();
}

void UiWarning::KeyCallback(GLFWwindow* window, int key, int scancode,
                            int action, int mods) {
  if (action == GLFW_RELEASE) {
    return;
  }
  GetGlfwContext(window)->ui_warning->Hide();
}

void UiWarning::MouseButtonCallback(GLFWwindow* window, int button, int action,
                                    int mods) {
  GetGlfwContext(window)->ui_warning->Hide();
}
