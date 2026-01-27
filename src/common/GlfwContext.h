#ifndef WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "FixedSizeQueue.h"

class Camera;
class TileRenderer;
class IUiMode;
class UiMenu;
class PickingFramebuffer;
class UiSharedResources;
class TextRenderer;
class UiConfirmation;
class UiWarning;
class UiRenderer;
class WindowQueue;
struct GLFWwindow;

namespace debug {

class UiDebugger;

}  // namespace debug

/** Because of the Glfw's nature for the most effective keyboard/mouse input
 * handling we should use callbacks that are C-function (global scope).
 * So we use provided by GLFW pointer void* GLFWwindow
 * */
struct GlfwContext {
  Camera* camera = nullptr;
  TileRenderer* tile_renderer = nullptr;
  IUiMode** cur_mode = nullptr;
  UiMenu* menu;
  PickingFramebuffer* picking_fbo = nullptr;
  debug::UiDebugger* ui_debugger = nullptr;
  UiSharedResources* ui_shared_resources = nullptr;
  TextRenderer* text_renderer = nullptr;
  UiRenderer* ui_renderer = nullptr;
  UiConfirmation* ui_confirmation = nullptr;
  UiWarning* ui_warning = nullptr;

  WindowQueue* windows = nullptr;

  glm::dvec2 cursor_pos_{0.0};
  glm::vec2 cursor_pos_tex_norm_{0.0f};

  GLuint hovered_id{0};

  void UpdateCursorPos();

  void UpdateHoveredId();

  [[nodiscard]] GLuint GetIdByMousePos(glm::vec2 mouse_pos) const;
};

GlfwContext* GetGlfwContext(GLFWwindow* window);

IUiMode* GetCurMode(GLFWwindow* window);

#endif  // WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
