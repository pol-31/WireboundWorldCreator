#include "WaterRenderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../io/Window.h"
#include "../common/ShadersBinding.h"

WaterRenderer::WaterRenderer(Tile& tile, const Paths& paths)
    : tile_(tile),
      shader_(paths.shader_water_vert, paths.shader_water_tesc,
              paths.shader_water_tese, paths.shader_water_frag),
      shader_picking_(paths.shader_height_map_picking_vert,
                      paths.shader_height_map_picking_frag) {
  Init();
}

void WaterRenderer::Render() const {
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

  shader_.Bind();
  glBindVertexArray(vao_);
  glActiveTexture(GL_TEXTURE0);
  tile_.map_water_height.Bind();
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void WaterRenderer::RenderPicking() const {
  shader_picking_.Bind();
  // TODO: 1024 * 1024 as an offset from Details.h
  glUniform1ui(shader::kHeightMapPickingIdOffset,
               static_cast<unsigned int>(1024 * 1024));
  glBindVertexArray(vao_);
  glActiveTexture(GL_TEXTURE0);
  tile_.map_water_height.Bind();
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1024 * 1024);
  glBindVertexArray(0);
}

glm::vec3 WaterRenderer::GetYPosition(int vertex_id) const {
  int coord_x = vertex_id & 1023;
  int coord_z = vertex_id >> 10;
  /*auto where =
      glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
  where.y = static_cast<float>(water_heights_[coord_x + coord_z * 1024]);
  where.y /= 64.0f; // TODO: idk why 64.0f*/
  //    return where;
  return {};
}

void WaterRenderer::Init() {
  //    return; //TODO: we use not "empty" but fill with tex_coords and pos !!!
  // Define the vertices (not actually used, but required for the draw call)
  GLfloat vertices[] = {
      -0.5f, 0.0f, -0.5f, 1.0f,
      0.5f, 0.0f, -0.5f, 1.0f,
      -0.5f, 0.0f,  0.5f, 1.0f,
      0.5f, 0.0f,  0.5f, 1.0f
  }; // TODO: its useless

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Enable the vertex attribute array
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0,
                        reinterpret_cast<void*>(0));

  shader_.Bind();
  glUniform1i(shader::kWaterHeightMap, 0);
}
