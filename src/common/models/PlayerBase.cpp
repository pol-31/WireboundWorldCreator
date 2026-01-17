#include "PlayerBase.h"

#include <glm/gtc/type_ptr.hpp>

#include "ModelLoader.h"

PlayerBase::PlayerBase(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources) {}

void PlayerBase::Render(UiSharedResources& ui_shared_resources) {
  UpdateAnimation();
  ui_shared_resources.shader_animated_mdl_.Bind();
  model_data_->BindTextures();
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}
