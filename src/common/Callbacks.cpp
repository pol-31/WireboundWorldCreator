#include "Callbacks.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include "../io/Camera.h"
#include "../io/Window.h"

void RemoveMouseJump() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
}

namespace callbacks {

void SetCameraCallbacks(bool mod_shift) {
  RemoveMouseJump();
  if (mod_shift) {
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_MmbShift);
  } else {
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_Mmb);
  }
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Mmb_MmbShift);
  glfwSetKeyCallback(gWindow, KeyCallback_Blocked);
}

void MouseButtonCallback_Mmb_MmbShift(GLFWwindow* window, int button,
                                      int action, int mods) {
  if (action == GLFW_PRESS ||
      (action == GLFW_RELEASE || button == GLFW_MOUSE_BUTTON_MIDDLE)) {
    // auto glfw_context =
        // reinterpret_cast<GlfwContext*>(glfwGetWindowUserPointer(window));
    // (*glfw_context->cur_mode)->BindDefaultCallbacks();
  }
}

void KeyCallback_Blocked(GLFWwindow* window, int key, int scancode, int action,
                         int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  // auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  // glfw_context->ui_renderer->Press(key, action);
  if (action == GLFW_RELEASE) {
    return;
  } else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_ESCAPE) {
    // (*glfw_context->cur_mode)->BindDefaultCallbacks();
  }
}

// camera view

void CursorPosCallback_Mmb(GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  std::cout << xpos << ' ' << ypos << std::endl;

  lastX = xpos;
  lastY = ypos;

  // auto game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
  // Camera* camera = game->camera_;
  // camera->MoveRotateViewOrigin(xoffset, yoffset);
}

void CursorPosCallback_MmbShift(GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 1.0f;
  float yoffset = (lastY - ypos) / 1.0f;

  lastX = xpos;
  lastY = ypos;

  // auto game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
  // Camera* camera = game->camera_;
  // camera->MovePanView(xoffset, yoffset);
}

}  // namespace callbacks
