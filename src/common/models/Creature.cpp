#include "Creature.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../io/Camera.h"
#include "../../core/TileRenderer.h"
#include "ModelLoader.h"

void Creature::Render(UiSharedResources& ui_shared_resources) {
  ui_shared_resources.shader_model_.Bind();
  model_data_->BindTextures();
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  if (selected_) {
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
  }
  glBindVertexArray(model_data_->vao);
  model_data_->RenderModelNodes();
  if (selected_) {
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    ui_shared_resources.shader_model_selected_.Bind();
    model = GenModelMat(ui_shared_resources, 0.11f);
    glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
    model_data_->RenderModelNodes();
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
  }
  glBindVertexArray(0);
}

void Creature::RenderPicking(UiSharedResources& ui_shared_resources) {
  ui_shared_resources.shader_model_picking_.Bind();
  int x = static_cast<int>(position_.x * 16.0f + 512.0f);
  int y = static_cast<int>(position_.z * 16.0f + 512.0f);
  GLuint lol_id = details::kIdOffsetWater + y * 1024 + x;
  glUniform1ui(1, static_cast<uint32_t>(lol_id));
  glBindVertexArray(model_data_->vao);
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

void Creature::Update(UiSharedResources& ui_shared_resources) {
  UpdatePosition(ui_shared_resources, glm::vec3{0.0f});
}
