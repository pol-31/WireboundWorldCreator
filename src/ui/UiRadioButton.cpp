#include "UiRadioButton.h"

const glm::vec4 UiRadioButton::gColorDefault = glm::vec4(1.0f);
const glm::vec4 UiRadioButton::gColorHovered =
    glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

UiRadioButton::UiRadioButton(data::UiId id, data::UiId& selected_id,
                             UiSprite& sp_selected, CallableType callback)
    : BaseType(id),
      selected_id_(selected_id),
      sp_selected_(sp_selected),
      callback_(std::move(callback)) {}

void UiRadioButton::Update(float delta_time, UiLayer& ui_layer,
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
  if (selected_id_ == id_) {
    quad = UiQuad::GenSprite(selected_id_, transform, color_);
    ui_layer.buffer_sprites.push_back(quad);
  }
  for (UiElement* e : mChildren)
    e->Update(delta_time, ui_layer, translate, rotate, scale);
}

void UiRadioButton::Press(unsigned int pressed_id) {
  callback_();
  auto id = static_cast<data::UiId>(pressed_id);
  if (id_ == id) {
    selected_id_ = id;
  }
}

void UiRadioButton::MoveMouse(glm::vec2 mouse_pos, unsigned int hovered_id) {
  color_ = gColorHovered;
}
