#ifndef WIREBOUNDWORLDCREATOR_SRC__SETUPWINDOW_H_
#define WIREBOUNDWORLDCREATOR_SRC__SETUPWINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Details.h"

#include "Camera.h"
#include "Terrain.h"
#include "GlobalGlfwCallbackData.h"

/// we made this variable global, because to GLFW internally uses functions
/// related to some global state/object, therefore callback functions or those,
/// based on obtaining window cursor position or switching mode among
/// glPolygonMode (fill or lines) and obtaining current pressed button, we
/// would need to pass it to almost every class as a field.
/// While this is possible, it brings complexity and redundant readability
/// issues.
/// Making it global is safe, because it handles internally by GLFW library
/// and call glfwTerminate() allows us to delete glfwWindow before main()
/// completes.
extern GLFWwindow* gWindow;

/// we also made values related to frame rate (delta time) global,
/// for the same reason. Moreover here it's absolutely safe
extern float lastX;
extern float lastY;
extern float delta_time;
extern float last_frame;

//TODO; maybe we can remove global variables, but this is not for now

/// initializes gWindow (see above)
void SetupWindow();

void CallbackFramebufferSize(GLFWwindow* window, int width, int height);

void CallbackScroll(GLFWwindow* window, double xoffset, double yoffset);

void CallbackCursorPos(GLFWwindow* window, double xpos, double ypos);

void CallbackMouseButton(GLFWwindow* window, int button,
                         int action, int mods);

void CallbackKey(GLFWwindow* window, int key,
                   int scancode, int action, int mods);

void ProcessInput(GLFWwindow* window, Camera& camera);

void APIENTRY glDebugOutput(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length [[maybe_unused]],
    const char *message, const void *user_param [[maybe_unused]]);

#endif  // WIREBOUNDWORLDCREATOR_SRC__SETUPWINDOW_H_
