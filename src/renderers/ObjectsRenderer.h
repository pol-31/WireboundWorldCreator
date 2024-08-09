#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_OBJECTSRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_OBJECTSRENDERER_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../io/Window.h"

#include <glm/glm.hpp>

#include "../core/Tile.h"

class ObjectsRenderer {
 public:
  ObjectsRenderer(Tile& tile) : tile_(tile) {}

  void Render() const {
    if(glfwGetKey(gWindow, GLFW_KEY_6)) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    //TODO: render objects here
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  void RenderPicking() const {}

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const {
    //TODO: but not for objects (object on object on object on object)
    //    int coord_x = vertex_id & 1023;
    //    int coord_z = vertex_id >> 10;
    /*auto where =
        glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
    where.y = static_cast<float>(terrain_heights_[coord_x + coord_z * 1024]);
    where.y /= 64.0f; // TODO: idk why 64.0f
    return where;*/
    return {};
  }

 private:
  Tile& tile_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_OBJECTSRENDERER_H_
