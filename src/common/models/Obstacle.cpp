#include "Obstacle.h"

#include <glm/gtc/type_ptr.hpp>

#include "ModelLoader.h"

void Obstacle::Render(UiSharedResources& ui_shared_resources) {
  ui_shared_resources.shader_model_.Bind();
  model_data_->BindTextures();
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  glBindVertexArray(model_data_->vao);
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

void Obstacle::Update(UiSharedResources& ui_shared_resources) {
  UpdatePosition(ui_shared_resources, glm::vec3{0.0f});
}
