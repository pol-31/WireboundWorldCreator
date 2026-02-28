#include "UiWorldOrigin.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../io/Camera.h"
#include "TileRenderer.h"

UiWorldOrigin::UiWorldOrigin(UiRenderData& render_data)
    : render_data_(render_data), sp_origin_(data::UiId::kPivot) {}

void UiWorldOrigin::Render(glm::vec4 position, glm::vec4 color) {
  render_data_.shader_sp_.Bind();
  glBindVertexArray(render_data_.vao_ui_);
  render_data_.tex_ui_.BindSampler(0);
  glUniform4fv(7, 1, glm::value_ptr(color));

  //  auto mvp = GetPointMvpMatrix();
  auto map_scale =
      render_data_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  //  std::cout << map_scale << " map_scale" << std::endl;
  auto model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(map_scale));
  auto view = render_data_.glfw_context_.camera->GetViewMatrix(map_scale);
  auto projection = render_data_.glfw_context_.camera->GetProjMatrix();
  auto mvp = projection * view * model;

  //  glm::vec2 translate = GetBillboardTranslate(mvp, i);
  glm::vec4 clipPos = mvp * position;
  glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

  sp_origin_.SetTranslate(glm::vec2(ndc.x, ndc.y));
  sp_origin_.Render();

  glm::vec4 color_white(1.0f);
  glUniform4fv(7, 1, glm::value_ptr(color_white));  // restore TODO: here?
}
