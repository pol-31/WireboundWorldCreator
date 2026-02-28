#include "UiSlider.h"

const glm::vec4 UiSliderH2::gColorDefault = glm::vec4(1.0f);
const glm::vec4 UiSliderH2::gColorHovered = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
const float UiSliderH2::gTrackLengthFactor = 0.9f;

UiSliderH2::UiSliderH2(data::UiId id_track, data::UiId id_handle)
    : BaseType(id_track), sp_handle_(id_handle) {
  Set(min_);
  Add(&sp_handle_);
  Add(&txt_text_);
}

// TODO: sometimes for Press() we also need press_id.. right? Then ur Press()
//  also contains int__pressed_id OR coordinate, so we Contains().... hmmmm
//  IT seeems we don't need fbo here... NO NONO NON ON , Let's add transparency
//  to ui so everything cool and if you press in alpha-o it just doesn't write
//  to depth/stencil ui picking fbo. Yes! Without it we can't implement it with
//  AABB-test

void UiSliderH2::Update(float delta_time, UiLayer& ui_layer,
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

  CachePosition(transform, scale);
  SetHandlePositionByValue();

  txt_text_.SetText(std::to_string(value_));

  for (UiElement* e : mChildren)
    e->Update(delta_time, ui_layer, translate, rotate, scale);
}

void UiSliderH2::CachePosition(const glm::mat3& transform, glm::vec2 scale) {
  // our sprite has atlas res * sprite res * window res (based on tex coords),
  // but initially we render it as -1;1 quad
  cached_length_ = scale.x * gTrackLengthFactor;
  cached_center_ = transform * glm::vec3(0.0f, 0.0f, 1.0f);  // ndc center
}

void UiSliderH2::Press(unsigned int pressed_id) { pressed_ = true; }

void UiSliderH2::Release() { pressed_ = false; }

// 1. id from picking fbo
// 2. we need to cache vars only, while set every frame in Update in case
// value changed by Set(some_value) so from the outside
void UiSliderH2::MoveMouse(glm::vec2 mouse_pos, unsigned int hovered_id) {
  if (hovered_id == static_cast<int>(id_)) {
    color_ = gColorHovered;
  } else {
    color_ = gColorDefault;
  }
  if (pressed_) {
    SetValueByMousePos(mouse_pos);
  }
}

void UiSliderH2::SetValueByMousePos(glm::vec2 mouse_pos) {
  glm::vec2 handle_position = cached_center_;  // start from track center
  auto half_length = cached_length_ / 2.0f;
  handle_position.x +=
      glm::clamp(mouse_pos.x - handle_position.x, -half_length, +half_length);
}

void UiSliderH2::SetHandlePositionByValue() {
  glm::vec2 handle_position = cached_center_;
  float progress = (value_ - min_) / (max_ - min_);
  float offset = cached_length_ * (progress - 0.5f);
  handle_position.x += offset;
  sp_handle_.SetPosition(handle_position);
}

void UiSliderH2::Scroll(unsigned int id, float yoffset) {
  Set(value_ + yoffset * 0.01f);
}

void UiSliderH2::Set(float value) { value_ = glm::clamp(value, min_, max_); }
