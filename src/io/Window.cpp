#include "Window.h"

float lastX = static_cast<float>(details::kWindowWidth) / 2.0;
float lastY = static_cast<float>(details::kWindowHeight) / 2.0;

float delta_time = 0.0f;
float last_frame = 0.0f;

GLFWwindow* gWindow = nullptr;

void CallbackFramebufferSize(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void CallbackScroll(GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  if (yoffset < 0.0f) {
    global_data->renderer_.DownScale();
  } else {
    global_data->renderer_.UpScale();
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


// TODO: each mode has its own versions of these two:
//   CallbackMouseButton, CallbackKey. BUT:
//  because GLFW needs function which is not a method (isn't it?), so
//  we need dynamic cast

// this callback is for terrain, so we dynamic cast it
void CallbackMouseButton(GLFWwindow* window, int button,
                         int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto* cur_mode = global_data->GetCurMode();
  auto terrain = dynamic_cast<TerrainMode>(cur_mode);
  //TODO: here related to TerrainMode functionality/calls like selecting point
  // and their moving, slider
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    void* global_data = glfwGetWindowUserPointer(window);
    Cursor& cursor = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->cursor_;
    cursor.SwitchMode(window);
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    // TODO: identify is it a button or terrain and then remember/ignore, etc...
    // TODO: identify is it a button or terrain and then remember/ignore, etc...
    // TODO: identify is it a button or terrain and then remember/ignore, etc...
    // TODO: identify is it a button or terrain and then remember/ignore, etc...
    // TODO: identify is it a button or terrain and then remember/ignore, etc...
    // TODO: identify is it a button or terrain and then remember/ignore, etc...
//    if (mouse_btn_pressed) { global_data.tab_pressed_? menu.Press() : cur_mode->Press()


    // we've updated cursor pos at Renderer::RunRenderLoop iteration,
    // so now just use it
    auto point_data = global_data->terrain.GetHeightId(
        global_data->renderer_.GetMousePos());
    terrain->SelectPoint(point_data);

  }
}
void CallbackKey(GLFWwindow* window, int key,
                 int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, true);
    } else if (key == GLFW_KEY_LEFT_SHIFT) {
      global_data->camera_.SpeedUp();
    } else if (key == GLFW_KEY_BACKSPACE) {
      global_data->terrain_.ClearPoints();
    } else if (key == GLFW_KEY_ENTER) {
      global_data->terrain_.Bake();
    } else if (key == GLFW_KEY_TAB) {
      global_data->tab_pressed_ = true;
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_LEFT_SHIFT) {
      global_data->camera_.SlowDown();
    } else if (key == GLFW_KEY_TAB) {
      global_data->tab_pressed_ = false;
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
  glfwSetKeyCallback(gWindow, CallbackKey);
  glfwSetMouseButtonCallback(gWindow, CallbackMouseButton);
  glfwSetCursorPosCallback(gWindow, CallbackCursorPos);
  glfwSetFramebufferSizeCallback(gWindow, CallbackFramebufferSize);
  glfwSetScrollCallback(gWindow, CallbackScroll);

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
