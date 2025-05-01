#include "Cursor.h"

#include "Window.h"

Cursor::Cursor()
    : last_x_(gWindowWidth / 2.0f),
      last_y_(gWindowHeight / 2.0f) {}

//TODO:
// bug#1 : disabled-cursor started position
// bug#2 : jump at slightly-lower mid (on y axis)

void Cursor::SetModeDisabled() {
  glfwSetCursorPosCallback(gWindow, CallbackCursorPos);
  glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(gWindow, last_x_, last_y_);
}

void Cursor::SetModeEnabled() {
  glfwGetCursorPos(gWindow, &last_x_, &last_y_);
  glfwSetCursorPosCallback(gWindow, nullptr);
  glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetCursorPos(gWindow, gWindowWidth / 2.0f, gWindowHeight / 2.0f);
}
