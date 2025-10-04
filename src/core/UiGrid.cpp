#include "UiGrid.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TileRenderer.h"
#include "../io/Cameras.h"

UiGrid::UiGrid(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      shader_("../shaders/TerrainGrid.vert",
              "../shaders/TerrainGrid.frag") {
  Init();
}

void UiGrid::Init() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  float quad_data[] = {
      16.0f, 0.0f, -16.0f,
      16.0f, 0.0f, 16.0f,
      -16.0f, 0.0f, -16.0f,
      -16.0f, 0.0f, 16.0f,
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

void UiGrid::Render() {
  shader_.Bind();
  auto map_scale = ui_shared_resources_.global_glfw_callback_data_
                       .tile_renderer->cur_tile_.map_scale;
  glUniform1f(1, map_scale);
  auto camera_pos = ui_shared_resources_.global_glfw_callback_data_
                        .camera->GetPosition();
  glUniform3fv(2, 1, glm::value_ptr(camera_pos));
//  std::cout << camera_pos.x << ' ' << camera_pos.z << ' ' << std::endl;
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


UiAxis::UiAxis(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      shader_("../shaders/Axis.vert",
              "../shaders/Axis.frag") {
  Init();
}

void UiAxis::Init() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  float quad_data[] = {
      16.0f, 0.0f, -0.01f,
      16.0f, 0.0f, +0.01f,
      -16.0f, 0.0f, -0.01f,
      -16.0f, 0.0f, +0.01f,

      0.0f, 16.0f, -0.01f,
      0.0f, 16.0f, +0.01f,
      0.0f, -16.0f, -0.01f,
      0.0f, -16.0f, +0.01f,

      -0.01f, 0.0f, 16.0f,
      +0.01f, 0.0f, 16.0f,
      -0.01f, 0.0f, -16.0f,
      +0.01f, 0.0f, -16.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void UiAxis::DeInit() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
}

void UiAxis::Render(float width_x, float width_y, float width_z) {
  glBindVertexArray(vao_);
  shader_.Bind();
//  auto map_scale = ui_shared_resources_.global_glfw_callback_data_
//                       .tile_renderer->cur_tile_.map_scale;
//  glUniform1f(1, map_scale);
  auto camera_pos = ui_shared_resources_.global_glfw_callback_data_
                        .camera->GetPosition();
  glUniform3fv(2, 1, glm::value_ptr(camera_pos));
  glm::vec3 color;
  color = glm::vec3(1.0f, 0.0f, 0.0f);
  glUniform3fv(3, 1, glm::value_ptr(color));
  glUniform1f(4, width_x);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  color = glm::vec3(0.0f, 1.0f, 0.0f);
  glUniform3fv(3, 1, glm::value_ptr(color));
  glUniform1f(4, width_y);
  glDrawArrays(GL_TRIANGLE_STRIP, 4, 8);
  color = glm::vec3(0.0f, 0.0f, 1.0f);
  glUniform3fv(3, 1, glm::value_ptr(color));
  glUniform1f(4, width_z);
  glDrawArrays(GL_TRIANGLE_STRIP, 8, 12);
}
