#include "UiCheckBox.h"

UiCheckBox::UiCheckBox(data::UiId id_on, data::UiId id_off)
    : BaseType(id_off), sprite_id_on_(id_on) {}

void UiCheckBox::Update(float delta_time, UiLayer& ui_layer,
                        glm::vec2 translate, float rotate, glm::vec2 scale) {
  translate += position_;
  rotate += rotate_;
  scale *= scale_;
  glm::mat3 transform(1.0f);
  transform *= glm::translate(transform, translate);
  transform *= glm::rotate(transform, rotate);
  transform *= glm::scale(transform, scale);
  auto quad = UiQuad::GenSprite(id_, transform, color_);
  ui_layer.buffer_sprites.push_back(quad);
  if (value_) {
    quad = UiQuad::GenSprite(sprite_id_on_, transform, color_);
    ui_layer.buffer_sprites.push_back(quad);
  }
  for (UiElement* e : mChildren)
    e->Update(delta_time, ui_layer, translate, rotate, scale);
}

void UiCheckBox::Press(unsigned int pressed_id) { value_ = !value_; }