#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_FENCESRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_FENCESRENDERER_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../io/Window.h"
#include <glm/glm.hpp>

#include "../core/Tile.h"

class FencesRenderer {
 public:
  FencesRenderer(Tile& tile) : tile_(tile) {}

  void Render() const {
    if (glfwGetKey(gWindow, GLFW_KEY_4)) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    // TODO: render fences here
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  void RenderPicking() const {}

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const {
    int coord_x = vertex_id & 1023;
    int coord_z = vertex_id >> 10;
    /*auto where =
        glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
    // y-coord + its height
    where.y = static_cast<float>(fences_[coord_x + coord_z * 1024].Top());
    where.y /= 64.0f; // TODO: idk why 64.0f*/
//    return where;
    return {};
  }

 private:
  Tile& tile_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_FENCESRENDERER_H_
