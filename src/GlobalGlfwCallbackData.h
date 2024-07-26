#ifndef WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_

#include "Camera.h"
#include "Cursor.h"
#include "Terrain.h"

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
class GlobalGlfwCallbackData {
 public:
  GlobalGlfwCallbackData(Camera& camera, Cursor& cursor, Terrain& terrain)
      : camera_(camera), cursor_(cursor), terrain_(terrain) {}
  Camera& camera_;
  Cursor& cursor_;
  Terrain& terrain_;
  /*
   * camera, resolution(settings), ...
   * all framebuffers
   * */
};

#endif //WIREBOUNDWORLDCREATOR_SRC_GLOBALGLFWCALLBACKDATA_H_
