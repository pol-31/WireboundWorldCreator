#include "UiWorldOrigin.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../io/Camera.h"
#include "TileRenderer.h"

UiWorldOrigin::UiWorldOrigin(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      sp_origin_(data::VboIdMain::kPivot) {}

void UiWorldOrigin::Render(glm::vec4 position, glm::vec4 color) {
  ui_shared_resources_.shader_sp_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.tex_ui_.Bind();

  glUniform4fv(7, 1, glm::value_ptr(color));

  //  auto mvp = GetPointMvpMatrix();
  auto map_scale =
      ui_shared_resources_.gltf_context_.tile_renderer->cur_tile_.map_scale;
  //  std::cout << map_scale << " map_scale" << std::endl;
  auto model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(map_scale));
  auto view =
      ui_shared_resources_.gltf_context_.camera->GetViewMatrix(map_scale);
  auto projection = ui_shared_resources_.gltf_context_.camera->GetProjMatrix();
  auto mvp = projection * view * model;

  //  glm::vec2 translate = GetBillboardTranslate(mvp, i);
  glm::vec4 clipPos = mvp * position;
  glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

  sp_origin_.SetTranslate(glm::vec2(ndc.x, ndc.y));
  sp_origin_.Render();

  glm::vec4 color_white(1.0f);
  glUniform4fv(7, 1, glm::value_ptr(color_white));  // restore TODO: here?
}
