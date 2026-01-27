#include "UiModal.h"

#include "../common/PickingFramebuffer.h"
#include "../modes/IUiMode.h"

UiConfirmation::UiConfirmation(UiSharedResources& ui_shared_resources,
                               TextRenderer& text_renderer)
    : ui_shared_resources_(ui_shared_resources),
      sp_background_(data::VboIdMain::kConfirmationDesk),
      sp_accept_(data::VboIdMain::kConfirmationAccept),
      sp_decline_(data::VboIdMain::kConfirmationDecline),
      text_(text_renderer, data::VboIdMain::kConfirmationText),
      hierarchy_(&sp_background_, {&sp_accept_, &sp_decline_, &text_}) {}

void UiConfirmation::Render() {
  if (!active_) {
    return;
  }
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  sp_background_.Render();
  sp_accept_.Render();
  sp_decline_.Render();
  text_.Render();
}

void UiConfirmation::RenderPicking() {
  if (!active_) {
    return;
  }
  ui_shared_resources_.shader_sp_picking_.Bind();
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
  (*ui_shared_resources_.glfw_context_.cur_mode)->Setup();
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

UiWarning::UiWarning(UiSharedResources& ui_shared_resources,
                     TextRenderer& text_renderer)
    : ui_shared_resources_(ui_shared_resources),
      sp_background_(data::VboIdMain::kCautionDesk),
      text_(text_renderer, data::VboIdMain::kCautionText),
      hierarchy_(&sp_background_, {&text_}) {}

void UiWarning::Render() {
  if (!active_) {
    return;
  }
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  sp_background_.Render();
  text_.Render();
}

void UiWarning::RenderPicking() {
  if (!active_) {
    return;
  }
  ui_shared_resources_.shader_sp_picking_.Bind();
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
  (*ui_shared_resources_.glfw_context_.cur_mode)->Setup();
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
