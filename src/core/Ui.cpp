#include "Ui.h"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "../modes/TerrainMode.h"
#include "../modes/WaterMode.h"
#include "../common/Colors.h"
#include "../common/ShadersBinding.h"

void UiDynamicSprite::Render() const {
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(transform_));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiDynamicSprite::RenderPicking() const {
  glUniform1ui(shader::kSpritePickingId, static_cast<uint32_t>(ui_data_.id));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

float UiDynamicSprite::GetLeftBorder() const {
  float width = details::kUiVboDataMain[ui_data_.vbo_offset * 4]
                - details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
  return transform_[3].x + width / 2;
}

float UiDynamicSprite::GetRightBorder() const {
  float width = details::kUiVboDataMain[ui_data_.vbo_offset * 4]
                - details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
  return transform_[3].x - width / 2;
}

float UiDynamicSprite::GetTopBorder() const {
  float height = details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                 - details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
  return transform_[3].y + height / 2;
}

float UiDynamicSprite::GetBottomBorder() const {
  float height = details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                 - details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
  return transform_[3].y - height / 2;
}

void UiStaticSprite::Render() const {
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiStaticSprite::RenderPicking() const {
  glUniform1ui(shader::kSpritePickingId, static_cast<uint32_t>(ui_data_.id));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

// see src/vbos/UiDataMain.h for explanation
float UiStaticSprite::GetLeftBorder() const {
  return details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
}

float UiStaticSprite::GetRightBorder() const {
  return details::kUiVboDataMain[ui_data_.vbo_offset * 4];
}

float UiStaticSprite::GetTopBorder() const {
  return details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5];
}

float UiStaticSprite::GetBottomBorder() const {
  return details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
}

UiHSlider::UiHSlider(UiDynamicSprite&& handle, UiDynamicSprite&& track,
                     glm::vec2 track_position, glm::vec2 handle_position,
                     float track_width)
    : UiSliderBase(std::move(handle), std::move(track)),
      handle_position_(handle_position),
      track_position_(track_position),
      width_(track_width) {
  glm::mat3 transform{1.0f};
  transform = glm::rotate(transform, glm::radians(90.0f));
  track_.SetTransform(glm::translate(transform, track_position));
  Set(glm::vec2(-2.0f, 0.0f));
}

void UiHSlider::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  track_.Render();
  handle_.Render();
}

void UiHSlider::Set(glm::vec2 mouse_pos) {
  float half_width_ = width_ / 2.0f;
  float offset = glm::clamp(mouse_pos.x - handle_position_.x,
                            -half_width_, +half_width_);
  glm::mat3 transform{1.0f};
  transform = glm::rotate(transform, glm::radians(90.0f));
  transform = glm::translate(transform, glm::vec2(offset, 0.0f));
  handle_.SetTransform(transform);
  progress_ = (offset + half_width_) / width_;
}

UiVSlider::UiVSlider(UiDynamicSprite&& handle, UiDynamicSprite&& track,
                     glm::vec2 track_position, glm::vec2 handle_position,
                     float track_height)
    : UiSliderBase(std::move(handle), std::move(track)),
      handle_position_(handle_position),
      track_position_(track_position),
      height_(track_height) {
  track_.SetTransform(glm::translate(glm::mat3{1.0f}, track_position));
  Set(glm::vec2(0.0f, -2.0f));
}

void UiVSlider::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  track_.Render();
  handle_.Render();
}

void UiVSlider::Set(glm::vec2 mouse_pos) {
  float half_heigth_ = height_ / 2.0f;
  float offset = glm::clamp(mouse_pos.y - handle_position_.y,
                            -half_heigth_, +half_heigth_);
  auto transform = glm::translate({1.0f}, glm::vec2(offset, 0.0f));
  handle_.SetTransform(transform);
  progress_ = (offset + half_heigth_) / height_;
}

UiSlots::UiSlots(const std::size_t& total_size, UiStaticSprite&& btn_next,
                 UiStaticSprite&& btn_prev, UiStaticSprite&& slot1,
                 UiStaticSprite&& slot2, UiStaticSprite&& slot3,
                 UiStaticSprite&& slot4, UiStaticSprite&& slot5,
                 int& edit_mode_selected_sample_id)
    : total_size_(total_size),
      btn_next_(btn_next),
      btn_prev_(btn_prev),
      slot1_(slot1),
      slot2_(slot2),
      slot3_(slot3),
      slot4_(slot4),
      slot5_(slot5),
      edit_mode_selected_sample_id_(edit_mode_selected_sample_id) {
  InitColors();
}

void UiSlots::Render() const {
  glUniform1f(shader::kSpriteBrightness, 0.5f);
  for (int i = 0; i < 5; ++i) {
    RenderSlot(i);
  }
  glUniform1f(shader::kSpriteBrightness, 1.0f);
  if (edit_mode_selected_sample_id_ != -1) {
    RenderSlot(edit_mode_selected_sample_id_);
  }
  RenderSlot(5);
  RenderSlot(6);
}

void UiSlots::RenderPicking() const {
  for (const auto& slot : slots_) {
    slot.Render();
  }
}

bool UiSlots::Press(std::uint32_t id) {
  // two last are btn_next, btn_prev
  if (id >= slots_[0].GetId() && id <= slots_[slots_.size() - 3].GetId()) {
    edit_mode_selected_sample_id_ = static_cast<int>(id);
  } else if (id == slots_[slots_.size() - 2].GetId()) {
    if (start_idx_ + 5 < total_size_) {
      ++start_idx_;
    }
  } else if (id == slots_[slots_.size() - 1].GetId()) {
    if (start_idx_ > 0) {
      --start_idx_;
    }
  } else {
    return false;
  }
  return true;
}

int UiSlots::Hover(std::uint32_t id) const {
  if (id >= slots_[0].GetId() && id <= slots_[slots_.size() - 1].GetId()) {
    return btn_next_.Hover(); // TODO: we need only one description
  }
  return {};
}

void UiSlots::InitColors() {
  colors_[0] = colors::kOrange;
  colors_[1] = colors::kGreen;
  colors_[2] = colors::kYellow;
  colors_[3] = colors::kRed;
  colors_[4] = colors::kBlue;
  colors_[5] = colors::kBrown;
  colors_[6] = colors::kMagenta;
  colors_[7] = colors::kLightGrey;
}

void UiSlots::RenderSlot(int i) const {
  if (total_size_ - 1 > start_idx_ + i) {
    int pos_in_slots = static_cast<int>(i & colors_.size());
    glUniform4fv(shader::kSpriteColor, 1,
                 glm::value_ptr(colors_[pos_in_slots]));
  } else {
    glUniform4fv(shader::kSpriteColor, 1, glm::value_ptr(colors::kBlack));
  }
  slots_[i].Render();
}

/*
UiBiomesList::UiBiomesList(
    int vbo_offset, int instances_num,
    UiButton&& btn_next, UiButton&& btn_prev)
    : vbo_offset_(vbo_offset),
      instances_num_(instances_num),
      points_data_(points_data),
      btn_next_(btn_next),
      btn_prev_(btn_prev),
      edit_mode_selected_sample_id_(edit_mode_selected_sample_id) {
  // vao has been already created at shared_resources initialization
}
*/

/*TODO:
 * instanced draw:
_pass_1: draw boxes (simply draw with the same sprites but different positions);
_pass_2: draw possible biomes (if less - simply don't draw - the same for picking)

_pass_picking: only _pass_2:
    */
void UiBiomesList::Render() const {
  btn_next_.Render();
  btn_prev_.Render();
//  TODO: vao_.Bind(); here?
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, vbo_offset_, 4, instances_num_);
}

void UiBiomesList::RenderPicking() const {
  //
}

/// return true if there was a button with such id
bool UiBiomesList::Press(std::uint32_t id) {
  //
}

int UiBiomesList::Hover(std::uint32_t id) const {
  //
}
