#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../io/Window.h"

#include "../core/Tile.h"

class PlacementRenderer {
 public:
  PlacementRenderer(Tile& tile) : tile_(tile) {}

  void Render() const {
    if(glfwGetKey(gWindow, GLFW_KEY_7)) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    //TODO: render placement here
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

 private:
  Tile& tile_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_PLACEMENTRENDERER_H_
