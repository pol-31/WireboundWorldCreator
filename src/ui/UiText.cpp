#include "UiText.h"

void UiText::Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
                    float rotate, glm::vec2 scale) {
  translate += position_;
  rotate += rotate_;
  scale *= scale_;
  glm::mat3 transform(1.0f);
  transform *= glm::translate(transform, translate);
  transform *= glm::rotate(transform, rotate);
  transform *= glm::scale(transform, scale);
  auto quad = UiQuad::GenText(transform, glm::vec4(1.0f));
  ui_layer.buffer_text.push_back(quad);
  ui_layer.text.push_back(text_);
  for (UiElement* e : mChildren)
    e->Update(delta_time, ui_layer, translate, rotate, scale);
}

void UiText::SetText(std::string_view text) { text_ = text; }
