#ifndef WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_

#include "../io/Camera.h"
#include "../io/Cursor.h"
#include "../io/Window.h"
#include "../core/TileRenderer.h"
#include "PickingFramebuffer.h"

//TODO:
// #include "../core/Menu.h"

class Menu;
class IEditMode;

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
  Camera& camera_; // camera movement
  Cursor& cursor_; // enable/disable cursor

  // handle input (e.g. call Press() of ui components), update maps scale
  TileRenderer& tile_renderer_;
  IEditMode*& cur_mode_;

  Menu& menu_;
  PickingFramebuffer& picking_fbo_;


  glm::dvec2 cursor_pos_{0.0};
  glm::vec2 cursor_pos_tex_norm_{0.0f};

  glm::ivec2 window_size_{details::kWindowWidth, details::kWindowHeight}; // TODO: init

  /*
   * camera, resolution(settings), ...
   * all framebuffers
   * */

  void UpdateCursorPos() {
    glfwGetCursorPos(gWindow, &cursor_pos_.x, &cursor_pos_.y);
    cursor_pos_tex_norm_ = {
        (cursor_pos_.x / window_size_.x) * 2.0f - 1,
        ((window_size_.y - cursor_pos_.y) / window_size_.y) * 2.0f - 1
    };
  }

  void UpdateWindowSize() {
    glfwGetWindowSize(gWindow, &window_size_.x, &window_size_.y);
  }

  GlobalGlfwCallbackData(Camera& camera, Cursor& cursor,
                         TileRenderer& tile_renderer, IEditMode*& cur_mode,
                         Menu& menu, PickingFramebuffer& picking_fbo)
      : camera_(camera),
        cursor_(cursor),
        tile_renderer_(tile_renderer),
        cur_mode_(cur_mode),
        menu_(menu),
        picking_fbo_(picking_fbo) {}
};

//TODO:
// - terrain scale
// - update other mode from TerrainMode::Bake()

#endif //WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
