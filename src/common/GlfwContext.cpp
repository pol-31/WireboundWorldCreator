#include "GlfwContext.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

GlfwContext* GetGlfwContext(GLFWwindow* window) {
  return reinterpret_cast<GlfwContext*>(glfwGetWindowUserPointer(window));
}
