#include "Cursor.h"

#include "Window.h"

void Cursor::SetModeDisabled() {
//  glfwGetCursorPos(gWindow, &disable_mode_last_x_, &disable_mode_last_y_);
  glfwSetCursorPosCallback(gWindow, nullptr);
  glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//  glfwSetCursorPos(gWindow, details::kWindowWidth / 2.0f,
//                   details::kWindowHeight / 2.0f);
}

void Cursor::SetModeEnabled() {
//  glfwSetCursorPos(gWindow, disable_mode_last_x_, disable_mode_last_y_);
  glfwSetCursorPosCallback(gWindow, CallbackCursorPos);
  glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
