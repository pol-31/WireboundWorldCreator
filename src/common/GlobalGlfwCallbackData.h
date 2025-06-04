#ifndef WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_

#include "../io/Cameras.h"
#include "../io/Cursor.h"
#include "../io/Window.h"
#include "../core/TileRenderer.h"
#include "PickingFramebuffer.h"
#include "UiDebugger.h"

//TODO:
// #include "../core/Menu.h"

class Menu;
class IEditMode;
class SharedResources;
class TextRenderer;

/** Because of the Glfw's nature for the most effective keyboard/mouse input
 * handling we should use callbacks that are C-function (global scope).
 * So to affect game data via callbacks we can either:
 * - make all our data global and access it with 0 overhead;
 * - use provided by GLFW pointer void* at GLFWwindow and access it via
 * reinterpret_cast with overhead of dereference on each such call.
 * Because dereferencing doesn't seem to be a huge problem we tend to use it,
 * otherwise we would pollute everything and make code less readable and less
 * maintainable.
 * All members are public for convenience; and are references.
 * */
struct GlobalGlfwCallbackData {
  CameraHandler& camera_; // camera movement
//  Cursor& cursor_; // enable/disable cursor

  // handle input (e.g. call Press() of ui components), update maps scale
  TileRenderer& tile_renderer_;
  IEditMode*& cur_mode_;

  Menu& menu_;
  PickingFramebuffer& picking_fbo_;

  debug::UiDebugger& ui_debugger_;

  glm::dvec2 cursor_pos_{0.0};
  glm::vec2 cursor_pos_tex_norm_{0.0f};

  SharedResources* shared_resources_{nullptr};

  TextRenderer* text_renderer_;

  void UpdateCursorPos();

  GlobalGlfwCallbackData(
      CameraHandler& camera/*, Cursor& cursor*/,
      TileRenderer& tile_renderer, IEditMode*& cur_mode,
      Menu& menu, PickingFramebuffer& picking_fbo,
      debug::UiDebugger& ui_debugger,
      SharedResources* shared_resources,
      TextRenderer* text_renderer);

  [[nodiscard]] GLuint GetIdByMousePos() const;

  void StartCharInput(FixedSizeQueue<char, 64>* name);

  void StopCharInput();
};

//TODO:
// - terrain scale
// - update other mode from TerrainMode::Bake()

#endif //WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
