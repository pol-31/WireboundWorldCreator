#include "Window.h"

#include <iostream>

#include "../common/Details.h"
#include "../common/GlobalGlfwCallbackData.h"

float lastX = static_cast<float>(details::kWindowWidth) / 2.0;
float lastY = static_cast<float>(details::kWindowHeight) / 2.0;

float gDeltaTime = 0.0f;
float last_frame = 0.0f;

GLFWwindow* gWindow = nullptr;

void CallbackFramebufferSize(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void CallbackCursorPos(GLFWwindow* window, double xpos, double ypos) {
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  void* global_data = glfwGetWindowUserPointer(window);
  Camera& camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera_;
  camera.ProcessMouseMovement(xoffset, yoffset);
}

/// wasd movement (for debugging purposes)
/// each IEditMode has its own set of callbacks (scroll, mouse / keyboard btn),
/// so they are bind at mode switching (Menu.h)
void WasdKeyCallback(GLFWwindow* window, int key,
                     int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_W) {
      global_data->camera_.SetMoveForward();
    } else if (key == GLFW_KEY_A) {
      global_data->camera_.SetMoveLeft();
    } else if (key == GLFW_KEY_S) {
      global_data->camera_.SetMoveBackward();
    } else if (key == GLFW_KEY_D) {
      global_data->camera_.SetMoveRight();
    } else if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, true);
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W) {
      global_data->camera_.UnSetMoveForward();
    } else if (key == GLFW_KEY_A) {
      global_data->camera_.UnSetMoveLeft();
    } else if (key == GLFW_KEY_S) {
      global_data->camera_.UnSetMoveBackward();
    } else if (key == GLFW_KEY_D) {
      global_data->camera_.UnSetMoveRight();
    }
  }
}

void APIENTRY glDebugOutput(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length [[maybe_unused]],
    const char *message, const void *user_param [[maybe_unused]]) {
  std::cout << "---------------"
            << "Debug message (" << id << "): " <<  message;

  std::cout << "\nSource: ";
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cout << "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cout << "Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cout << "Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cout << "Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      std::cout << "Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      std::cout << "Other";
      break;
    default:
      std::cout << "undefined";
  }

  std::cout << "\nType: ";
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cout << "Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cout << "Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cout << "Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      std::cout << "Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cout << "Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      std::cout << "Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      std::cout << "Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      std::cout << "Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      std::cout << "Other";
      break;
    default:
      std::cout << "undefined";
  }

  std::cout << "\nSeverity: ";
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      std::cout << "notification";
      break;
    default:
      std::cout << "undefined";
  }
  std::cout << '\n';
  std::cout << std::endl;
}

void SetupWindow() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 8);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  gWindow = glfwCreateWindow(
      details::kWindowWidth, details::kWindowHeight,
      "WireboundDev", nullptr, nullptr);
  if (!gWindow) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(gWindow);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
  glfwSetCursorPosCallback(gWindow, CallbackCursorPos);
  glfwSetFramebufferSizeCallback(gWindow, CallbackFramebufferSize);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                          0, nullptr, GL_TRUE);
  }
  glDebugMessageControl(GL_DEBUG_SOURCE_API,
                        GL_DEBUG_TYPE_ERROR,
                        GL_DEBUG_SEVERITY_HIGH,
                        0, nullptr, GL_TRUE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}
