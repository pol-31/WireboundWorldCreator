#include "UiButton.h"

const glm::vec4 UiButton::gColorDefault = glm::vec4(1.0f);
const glm::vec4 UiButton::gColorHovered = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

UiButton::UiButton(data::UiId id, CallableType callback)
    : BaseType(id), callback_(std::move(callback)) {}

void UiButton::Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
                      float rotate, glm::vec2 scale) {
  translate += position_;
  rotate += rotate_;
  scale *= scale_;
  glm::mat3 transform(1.0f);
  transform *= glm::translate(transform, translate);
  transform *= glm::rotate(transform, rotate);
  transform *= glm::scale(transform, scale);
  auto quad = UiQuad::GenSprite(id_, transform, color_);
  ui_layer.buffer_sprites.push_back(quad);
  for (UiElement* e : mChildren)
    e->Update(delta_time, ui_layer, translate, rotate, scale);
}

void UiButton::Press(unsigned int pressed_id) { callback_(); }

void UiButton::MoveMouse(glm::vec2 mouse_pos, unsigned int hovered_id) {
  color_ = gColorHovered;
}
