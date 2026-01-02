#include "Callbacks.h"

#define GLFW_INCLUDE_NONE
#include "../io/Camera.h"
#include "../io/Window.h"
#include "../modes/IUiMode.h"
#include "../renderers/UiRenderer.h"
#include "GlfwContext.h"
#include "glfw/glfw3.h"

namespace callbacks {

void MouseButtonCallback_Mmb_MmbShift(GLFWwindow* window, int button,
                                      int action, int mods) {
  if (action == GLFW_PRESS ||
      (action == GLFW_RELEASE || button == GLFW_MOUSE_BUTTON_MIDDLE)) {
    auto glfw_context =
        reinterpret_cast<GlfwContext*>(glfwGetWindowUserPointer(window));
    (*glfw_context->cur_mode)->BindDefaultCallbacks();
  }
}

void KeyCallback_Blocked(GLFWwindow* window, int key, int scancode, int action,
                         int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  glfw_context->ui_renderer->Press(key, action);
  if (action == GLFW_RELEASE) {
    return;
  } else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_ESCAPE) {
    (*glfw_context->cur_mode)->BindDefaultCallbacks();
  }
}

// camera view

void CursorPosCallback_Mmb(GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  lastX = xpos;
  lastY = ypos;

  Camera* camera = GetGlfwContext(window)->camera;
  camera->MoveRotateViewOrigin(xoffset, yoffset);
}

void CursorPosCallback_MmbShift(GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 1.0f;
  float yoffset = (lastY - ypos) / 1.0f;

  lastX = xpos;
  lastY = ypos;

  Camera* camera = GetGlfwContext(window)->camera;
  camera->MovePanView(xoffset, yoffset);
}

}  // namespace callbacks
