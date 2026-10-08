#include "Window.h"

#include <iostream>

#include "../common/Details.h"
#include "../common/GlfwContext.h"
#include "../common/PickingFramebuffer.h"
#include "Camera.h"

#include "../core/Renderer.h"
#include "../ui/DebugUI.h"

int gWindowWidth = 1600;
int gWindowHeight = 900;
float gResFactor =
    static_cast<float>(gWindowHeight) / static_cast<float>(gWindowWidth);

float lastX = static_cast<float>(gWindowWidth) / 2.0;
float lastY = static_cast<float>(gWindowHeight) / 2.0;

float gDeltaTime = 0.0f;
float gDeltaTimePhysics = 0.0f;
float last_frame = 0.0f;

double gEventMouseStartPosX = 0.0;
double gEventMouseStartPosY = 0.0;

// TODO: bear out to another class/struct

GLFWwindow* gWindow = nullptr;

// Store windowed position and size before entering fullscreen so you can restore later
// int windowedX = 100, windowedY = 100;
// int windowedWidth = 1280, windowedHeight = 720;

void SetFullscreen(bool enableFullscreen) {
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
  if (enableFullscreen) {
    glfwSetWindowMonitor(gWindow, primaryMonitor, 0, 0, gWindowWidth, gWindowHeight, GLFW_DONT_CARE);
  } else {
    glfwSetWindowMonitor(gWindow, nullptr, 0, 0, gWindowWidth, gWindowHeight, GLFW_DONT_CARE);
    CenterWindow();
  }
}

void CenterWindow() {
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  if (!primaryMonitor) {
    return;
  }
  int monitor_x, monitor_y, monitor_width, monitor_height;
  glfwGetMonitorWorkarea(primaryMonitor, &monitor_x, &monitor_y,
    &monitor_width, &monitor_height);
  int xPos = monitor_x + (monitor_width - gWindowWidth) / 2;
  int yPos = monitor_y + (monitor_height - gWindowHeight) / 2;
  glfwSetWindowPos(gWindow, xPos, yPos);
}

void CallbackFramebufferSize(GLFWwindow* window, int width, int height) {
  auto resolution = glm::ivec2{width, height};
  gWindowWidth = resolution.x;
  gWindowHeight = resolution.y;
  gResFactor =
      static_cast<float>(gWindowHeight) / static_cast<float>(gWindowWidth);
  glViewport(0, 0, resolution.x, resolution.y);
  gResFactor = static_cast<float>(gWindowHeight) / static_cast<float>(gWindowWidth);
  lastX = static_cast<float>(gWindowWidth) / 2.0;
  lastY = static_cast<float>(gWindowHeight) / 2.0;
  CenterWindow();
  auto glfw_context =
      reinterpret_cast<GlfwContext*>(glfwGetWindowUserPointer(gWindow));
  glfw_context->renderer->OnWindowResize();
  glfw_context->ui_manager->OnWindowResize();
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length [[maybe_unused]],
                            const char* message,
                            const void* user_param [[maybe_unused]]) {
  std::cout << "---------------"
            << "Debug message (" << id << "): " << message;

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

  gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, "WireboundDev",
                             nullptr, nullptr);
  CenterWindow();
  if (!gWindow) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(gWindow);
  glfwSetKeyCallback(gWindow, nullptr);
  glfwSetCursorPosCallback(gWindow, nullptr);
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
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }
  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR,
                        GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
  // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

WindowResolution gWindowResolutions[] = {
  {640, 480},   // VGA
  {800, 600},   // SVGA
  {1024, 768},  // XGA
  {1280, 720},  // HD
  {1600, 900},  // HD+
  {1920, 1080}  // Full HD
};

std::vector<std::string> GenWindowResolutionStrings() {
  std::string str_resolution;
  str_resolution.reserve(11);
  std::string str_resolution_delim = " x ";
  std::vector<std::string> resolutions(std::size(gWindowResolutions), str_resolution);
  for (int i = 0; i < resolutions.size(); i++) {
    auto& res = gWindowResolutions[i];
    auto& str = resolutions[i];
    str += std::to_string(res.width);
    str += str_resolution_delim;
    str += std::to_string(res.height);
  }
  return resolutions;
}

WindowResolution GetWindowResolution(int id) {
  return gWindowResolutions[id];
}

int GetDefaultWindowResolutionId() {
  return 4;
}
