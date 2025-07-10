#ifndef WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "FixedSizeQueue.h"

class CameraHandler;
class TileRenderer;
class IUiMode;
class Menu;
class PickingFramebuffer;
class UiSharedResources;
class TextRenderer;
class WindowQueue;

namespace debug {

class UiDebugger;

} // namespace debug

/** Because of the Glfw's nature for the most effective keyboard/mouse input
 * handling we should use callbacks that are C-function (global scope).
 * So we use provided by GLFW pointer void* GLFWwindow
 * */
struct GlobalGlfwCallbackData {
  CameraHandler* camera{nullptr};
  TileRenderer* tile_renderer{nullptr};
  IUiMode** cur_mode{nullptr};
  Menu* menu;
  PickingFramebuffer* picking_fbo{nullptr};
  debug::UiDebugger* ui_debugger{nullptr};
  UiSharedResources* ui_shared_resources{nullptr};
  TextRenderer* text_renderer{nullptr};
  WindowQueue* windows{nullptr};

  glm::dvec2 cursor_pos_{0.0};
  glm::vec2 cursor_pos_tex_norm_{0.0f};

  void UpdateCursorPos();

  [[nodiscard]] GLuint GetIdByMousePos() const;
};

#endif //WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
