#include "UiGrid.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../io/Camera.h"
#include "TileRenderer.h"

UiGrid::UiGrid(UiRenderData& render_data)
    : render_data_(render_data),
      shader_grid_("../shaders/TerrainGrid.vert",
                   "../shaders/TerrainGrid.frag", {}),
      shader_axis_("../shaders/TerrainGrid.vert", "../shaders/Axis.frag", {}),
      shader_world_boundary_("../shaders/TerrainGrid.vert",
                             "../shaders/TerrainBoundary.frag", {}) {
  Init();
}

void UiGrid::Init() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  float quad_data[] = {
      16.0f,  0.001f, -16.0f, 16.0f,  0.001f, 16.0f,
      -16.0f, 0.001f, -16.0f, -16.0f, 0.001f, 16.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void UiGrid::DeInit() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
}

void UiGrid::RenderGrid() {
  glBindVertexArray(vao_);
  shader_grid_.Bind();
  auto map_scale =
      render_data_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  glUniform1f(1, map_scale);
  auto camera_pos = render_data_.glfw_context_.camera->GetPosition();
  glUniform3fv(2, 1, glm::value_ptr(camera_pos));
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void UiGrid::RenderAxis(float scale) {
  glBindVertexArray(vao_);
  shader_axis_.Bind();
  auto camera_pos = render_data_.glfw_context_.camera->GetPosition();
  glUniform3fv(0, 1, glm::value_ptr(camera_pos));
  glUniform1f(1, scale);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void UiGrid::RenderBoundary() {
  glBindVertexArray(vao_);
  shader_world_boundary_.Bind();
  auto map_scale =
      render_data_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  glUniform1f(1, map_scale);
  auto camera_pos = render_data_.glfw_context_.camera->GetPosition();
  glUniform3fv(2, 1, glm::value_ptr(camera_pos));
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
