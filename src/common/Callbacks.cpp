#include "Callbacks.h"

#define GLFW_INCLUDE_NONE
#include "glfw/glfw3.h"

#include "../io/Window.h"
#include "../io/Camera.h"

#include "GlobalGlfwCallbackData.h"
#include "../renderers/UiRenderer.h"
#include "../modes/IUiMode.h"

namespace callbacks {

void MouseButtonCallback_Mmb_MmbShift(
    GLFWwindow* window, int button, int action, int mods) {
  if (action == GLFW_PRESS ||
      (action == GLFW_RELEASE ||
       button == GLFW_MOUSE_BUTTON_MIDDLE)) {
    auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
        glfwGetWindowUserPointer(window));
    (*global_data->cur_mode)->BindDefaultCallbacks();
  }
}

void KeyCallback_Blocked(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  global_data->ui_renderer->Press(key, action);
  if (action == GLFW_RELEASE) {
    return;
  } else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_ESCAPE) {
    (*global_data->cur_mode)->BindDefaultCallbacks();
  }
}

// camera view

void CursorPosCallback_Mmb(
    GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  lastX = xpos;
  lastY = ypos;

  void* global_data = glfwGetWindowUserPointer(window);
  Camera* camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera;
  camera->MoveRotateViewOrigin(xoffset, yoffset);
}

void CursorPosCallback_MmbShift(
    GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 1.0f;
  float yoffset = (lastY - ypos) / 1.0f;

  lastX = xpos;
  lastY = ypos;

  void* global_data = glfwGetWindowUserPointer(window);
  Camera* camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera;
  camera->MovePanView(xoffset, yoffset);
}

} // namespace callbacks
