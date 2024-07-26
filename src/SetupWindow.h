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

extern float lastX;
extern float lastY;

extern double prev_mode_last_x;
extern double prev_mode_last_y;

extern bool firstMouse;

extern float delta_time;
extern float last_frame;

extern float world_scale;


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

GLFWwindow* SetupWindow();

#endif  // WIREBOUNDWORLDCREATOR_SRC__SETUPWINDOW_H_
