#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_

#include <glm/glm.hpp>

#include "../core/Tile.h"

class WaterRenderer {
 public:
  WaterRenderer(Tile& tile) : tile_(tile) {}

  void Render() const {
    if(glfwGetKey(gWindow, GLFW_KEY_2)) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    //TODO: render water here
    /*    for (const auto& water : all_separate_water) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_water_points_);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                        water.size() * sizeof(float), water.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glDrawArrays(GL_POINTS, 0, water.size());
        //TODO: from each point we render triangle in geom shader?
        // SO it looks like we need: tessellation for borders (randomized
        //  cubic spline), tessellation for waves (quads);
        //  DO WE need geometry shader to create triangles out of points,
        //  or we can do this with patches in tessellation shader?
      }*/
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  void RenderPicking() const {}

  [[nodiscard]] glm::vec3 GetYPosition(int vertex_id) const {
    int coord_x = vertex_id & 1023;
    int coord_z = vertex_id >> 10;
    /*auto where =
        glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
    where.y = static_cast<float>(water_heights_[coord_x + coord_z * 1024]);
    where.y /= 64.0f; // TODO: idk why 64.0f*/
//    return where;
    return {};
  }

 private:
  Tile& tile_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATERRENDERER_H_
