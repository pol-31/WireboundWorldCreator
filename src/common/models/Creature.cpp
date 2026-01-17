#include "Creature.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../../io/Camera.h"
#include "ModelLoader.h"

Creature::Creature() { id_ = gEntityIdManager.Allocate(); }

void Creature::Render(UiSharedResources& ui_shared_resources) {
  UpdateAnimation();
  ui_shared_resources.shader_animated_mdl_.Bind();
  model_data_->BindTextures();
  auto model = GenModelMat(ui_shared_resources, 0.1f);
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
    ui_shared_resources.shader_animated_mdl_selected_.Bind();
    model = GenModelMat(ui_shared_resources, 0.11f);
    glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
    model_data_->RenderModelNodes();
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
  }
  glBindVertexArray(0);
}

void Creature::Update(UiSharedResources& ui_shared_resources) {
  UpdatePosition(ui_shared_resources, glm::vec3{0.0f});
}
