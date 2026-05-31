#include "Window.h"

#include <iostream>

#include "../common/Details.h"
#include "../common/GlfwContext.h"
#include "../common/PickingFramebuffer.h"
#include "Camera.h"

int gWindowWidth = 1600;
int gWindowHeight = 900;
float gResFactor =
    static_cast<float>(gWindowHeight) / static_cast<float>(gWindowWidth);

float lastX = static_cast<float>(gWindowWidth) / 2.0;
float lastY = static_cast<float>(gWindowHeight) / 2.0;

float gDeltaTime = 0.0f;
float last_frame = 0.0f;

double gEventMouseStartPosX = 0.0;
double gEventMouseStartPosY = 0.0;

// TODO: bear out to another class/struct

GLFWwindow* gWindow = nullptr;

void CallbackFramebufferSize(GLFWwindow* window, int width, int height) {
  auto resolution = SetWindowSize({width, height});
  gWindowWidth = resolution.x;
  gWindowHeight = resolution.y;
  gResFactor =
      static_cast<float>(gWindowHeight) / static_cast<float>(gWindowWidth);
  glViewport(0, 0, resolution.x, resolution.y);
  glfwSetWindowSize(gWindow, resolution.x, resolution.y);

  auto glfw_context =
      reinterpret_cast<GlfwContext*>(glfwGetWindowUserPointer(gWindow));
  // glfw_context->camera->UpdateProjectionMatrix();
  // glfw_context->ui_debugger->UpdateMoveSteps();
  // glfw_context->picking_fbo->UpdateResolution();
  // glfw_context->renderer->OnWindowResize();
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
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

glm::ivec2 ChooseNearestSize(glm::ivec2 size) {
  // 6 most common resolutions up to 1920x1080
  std::array<glm::ivec2, 6> resolutions = {
      glm::ivec2(640, 480),   // VGA
      glm::ivec2(800, 600),   // SVGA
      glm::ivec2(1024, 768),  // XGA
      glm::ivec2(1280, 720),  // HD
      glm::ivec2(1600, 900),  // HD+
      glm::ivec2(1920, 1080)  // Full HD
  };

  auto closest_it = std::min_element(
      resolutions.begin(), resolutions.end(),
      [size](const glm::ivec2& a, const glm::ivec2& b) {
        glm::ivec2 da = a - size;
        glm::ivec2 db = b - size;
        return (da.x * da.x + da.y * da.y) < (db.x * db.x + db.y * db.y);
      });

  // can't be nullptr
  glm::ivec2 result_size = *closest_it;
  std::cout << "Closest resolution is " << result_size.x << " x "
            << result_size.y << std::endl;
  return result_size;
}

glm::ivec2 SetWindowSize(glm::ivec2 size) {
  auto resolution = ChooseNearestSize(size);
  // TODO: window, viewport, notify all framebuffers, buttons
  return resolution;
}
