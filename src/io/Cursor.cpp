#include "Cursor.h"

#include "Window.h"

void Cursor::SetModeDisabled(GLFWwindow* window) {
  glfwGetCursorPos(window, &disable_mode_last_x_, &disable_mode_last_y_);
  glfwSetCursorPosCallback(window, nullptr);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetCursorPos(window, details::kWindowWidth / 2.0f,
                   details::kWindowHeight / 2.0f);
  // TODO: looks like need to update camera default yaw/pitch or
  //  front vector for smooth transition and what I wanted
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
