#include "GlfwContext.h"

#define GLFW_INCLUDE_NONE
#include "../common/TextRenderer.h"
#include "../modes/IUiMode.h"
#include "GLFW/glfw3.h"
#include "PickingFramebuffer.h"

void GlfwContext::UpdateCursorPos() {
  glfwGetCursorPos(gWindow, &cursor_pos_.x, &cursor_pos_.y);
  cursor_pos_tex_norm_ = {
      (cursor_pos_.x / gWindowWidth) * 2.0f - 1.0f,
      ((gWindowHeight - cursor_pos_.y) / gWindowHeight) * 2.0f - 1.0f};
}

void GlfwContext::UpdateHoveredId() {
  hovered_id = GetIdByMousePos(cursor_pos_);
}

GLuint GlfwContext::GetIdByMousePos(glm::vec2 mouse_pos) const {
  return picking_fbo->GetIdByMousePos(mouse_pos);
}

GlfwContext* GetGlfwContext(GLFWwindow* window) {
  return reinterpret_cast<GlfwContext*>(glfwGetWindowUserPointer(window));
}

IUiMode* GetCurMode(GLFWwindow* window) {
  auto glfw_context = GetGlfwContext(window);
  return *glfw_context->cur_mode;
}
