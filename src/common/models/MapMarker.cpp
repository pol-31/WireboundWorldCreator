#include "MapMarker.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../Details.h"
#include "ModelLoader.h"

void MapMarker::Render(UiSharedResources& ui_shared_resources, glm::vec3 color,
                       glm::vec2 position, glm::quat rotation,
                       glm::vec3 scale) {
  ui_shared_resources.shader_mdl_color_.Bind();
  glUniform3fv(1, 1, glm::value_ptr(color));
  model_data_->BindTextures();
  auto model =
      GenModelMat(ui_shared_resources, position, rotation, scale * 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  if (selected_) {
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
  }
  model_data_->RenderModelNodes();
  if (selected_) {
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    ui_shared_resources.shader_mdl_selected_.Bind();
    model = GenModelMat(ui_shared_resources, position, rotation, scale * 0.11f);
    glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
    model_data_->RenderModelNodes();
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
  }
  glBindVertexArray(0);
}

void MapMarker::RenderPicking(UiSharedResources& ui_shared_resources, int id,
                              glm::vec2 position, glm::quat rotation,
                              glm::vec3 scale) {
  ui_shared_resources.shader_mdl_picking_.Bind();
  glUniform1ui(1, static_cast<uint32_t>(id));
  auto model =
      GenModelMat(ui_shared_resources, position, rotation, scale * 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

glm::mat4 MapMarker::GenModelMat(UiSharedResources& ui_shared_resources,
                                 glm::vec2 position, glm::quat rotation,
                                 glm::vec3 scale) {
  auto fx = static_cast<int>(position.x * 16.0f + 512.0f);
  auto fz = static_cast<int>(position.y * 16.0f + 512.0f);
  float ground_height =
      ui_shared_resources.glfw_context_.tile_renderer->cur_tile_.GetPositionY(
          fx, fz);
  auto map_scale =
      ui_shared_resources.glfw_context_.tile_renderer->cur_tile_.map_scale;
  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(
      object_model, glm::vec3(position.x, ground_height, position.y));
  object_model *= glm::mat4_cast(rotation);
  object_model = glm::scale(object_model, scale);
  glm::mat4 map_model =
      glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));  // upscaled
  return map_model * object_model;
}
