#include "SetupWindow.h"

float lastX = static_cast<float>(details::kWindowWidth) / 2.0;
float lastY = static_cast<float>(details::kWindowHeight) / 2.0;

float delta_time = 0.0f;
float last_frame = 0.0f;

float world_scale = 1.0f;


void CallbackFramebufferSize(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

/*
void CallbackMouseButton(GLFWwindow* window, int button,
                         int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    //
  }
}
*/

void CallbackScroll(GLFWwindow* window, double xoffset, double yoffset) {
  if (yoffset < 0.0f) {
    world_scale /= 1.2f;
  } else {
    world_scale *= 1.2f;
  }
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

void CallbackMouseButton(GLFWwindow* window, int button,
                         int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    void* global_data = glfwGetWindowUserPointer(window);
    Cursor& cursor = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->cursor_;
    cursor.SwitchMode(window);
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    void* global_data = glfwGetWindowUserPointer(window);
    Terrain& terrain = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->terrain_;
    double pos_x, pos_y;
    glfwGetCursorPos(window, &pos_x, &pos_y);
    auto point_data = terrain.GetHeightId(pos_x, pos_y);
    terrain.SetId(point_data);
  }
}

void CallbackKey(GLFWwindow* window, int key,
                 int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, true);
    } else if (key == GLFW_KEY_1) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else if (key == GLFW_KEY_2) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else if (key == GLFW_KEY_LEFT_SHIFT) {
      void* global_data = glfwGetWindowUserPointer(window);
      Camera& camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera_;
      camera.SpeedUp();
    } else if (key == GLFW_KEY_BACKSPACE) {
      void* global_data = glfwGetWindowUserPointer(window);
      Terrain& terrain = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->terrain_;
      terrain.ClearPoints();
    } else if (key == GLFW_KEY_ENTER) {
      void* global_data = glfwGetWindowUserPointer(window);
      Terrain& terrain = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->terrain_;
      terrain.Bake();
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_LEFT_SHIFT) {
      void* global_data = glfwGetWindowUserPointer(window);
      Camera& camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera_;
      camera.SlowDown();
    }
  }
}

void ProcessInput(GLFWwindow* window, Camera& camera) {
  float velocity = camera.GetSpeed() * delta_time;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.MoveForward(velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.MoveBackward(velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.MoveLeft(velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.MoveRight(velocity);
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

GLFWwindow* SetupWindow() {
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

  GLFWwindow* window = glfwCreateWindow(
      details::kWindowWidth, details::kWindowHeight,
      "WireboundDev", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, CallbackKey);
  glfwSetMouseButtonCallback(window, CallbackMouseButton);
  glfwSetCursorPosCallback(window, CallbackCursorPos);
  glfwSetFramebufferSizeCallback(window, CallbackFramebufferSize);
  glfwSetScrollCallback(window, CallbackScroll);

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

  glEnable(GL_MULTISAMPLE);

  return window;
}
