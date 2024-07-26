#include "Cursor.h"

#include "SetupWindow.h"

void Cursor::SetModeDisabled(GLFWwindow* window) {
  glfwGetCursorPos(window, &disable_mode_last_x_, &disable_mode_last_y_);
  glfwSetCursorPosCallback(window, nullptr);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetCursorPos(window, details::kWindowWidth / 2.0f,
                   details::kWindowHeight / 2.0f);
  // TODO: looks like need to update camera default yaw/pitch or front vector for
  //  smooth transition and what I wanted
  //  smooth transition and what I wanted
  //  smooth transition and what I wanted
  //  smooth transition and what I wanted
  //  smooth transition and what I wanted
  enabled_ = false;
}
void Cursor::SetModeEnabled(GLFWwindow* window) {
  glfwSetCursorPos(window, disable_mode_last_x_, disable_mode_last_y_);
  glfwSetCursorPosCallback(window, CallbackCursorPos);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  enabled_ = true;
}
void Cursor::SwitchMode(GLFWwindow* window) {
  if (enabled_) {
    SetModeDisabled(window);
  } else {
    SetModeEnabled(window);
  }
}

void Cursor::Render(const Shader& shader, GLuint vao_id, GLuint vbo_id) {
  if (enabled_) {
    return;
  }
  glBindVertexArray(vao_id);
  shader.Bind();
  glActiveTexture(GL_TEXTURE0);
  tex_.Bind();
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0, details::kTexPositionSize,
      details::kTexPositionCursor.data());
  glBufferSubData(
      GL_ARRAY_BUFFER, details::kTexPositionSize,
      details::kTexCoordsSize, details::kTexCoordsDefault.data());
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}