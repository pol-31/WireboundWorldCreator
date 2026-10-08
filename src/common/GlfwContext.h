#ifndef WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_

class Renderer;
class UiRenderer;
class DebugUI;
class GLFWwindow;
class Game;

struct GlfwContext {
  Renderer* renderer = nullptr;
  DebugUI* ui_manager = nullptr;
  Game* game = nullptr;
};

GlfwContext* GetGlfwContext(GLFWwindow* window);

#endif  // WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
