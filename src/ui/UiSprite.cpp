#include "UiSprite.h"

void UiSprite::Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
                      float rotate, glm::vec2 scale) {
  translate += position_;
  rotate += rotate_;
  scale *= scale_;
  glm::mat3 transform(1.0f);
  transform *= glm::translate(transform, translate);
  transform *= glm::rotate(transform, rotate);
  transform *= glm::scale(transform, scale);
  auto quad = UiQuad::GenSprite(id_, transform, glm::vec4(1.0f));
  ui_layer.buffer_sprites.push_back(quad);
  for (UiElement* e : mChildren)
    e->Update(delta_time, ui_layer, translate, rotate, scale);
}
