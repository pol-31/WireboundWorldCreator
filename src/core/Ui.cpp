#include "Ui.h"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "../modes/TerrainMode.h"
#include "../modes/WaterMode.h"
#include "../common/Colors.h"
#include "../common/ShadersBinding.h"

void UiButton::Render() const {
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiButton::RenderPicking(const Shader& picking_shader) const {
  glUniform1ui(shader::kSpritePickingId, static_cast<uint32_t>(ui_data_.id));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

// see src/vbos/UiDataMain.h for explanation
float UiButton::GetLeftBorder() const {
  return details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
}

float UiButton::GetRightBorder() const {
  return details::kUiVboDataMain[ui_data_.vbo_offset * 4];
}

float UiButton::GetTopBorder() const {
  return details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5];
}

float UiButton::GetBottomBorder() const {
  return details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
}

void UiSliderHandle::Render(const Shader& slider_shader) const {
  slider_shader.Bind();
  glUniform2fv(shader::kDynamicSpriteScale, 1,
               glm::value_ptr(glm::vec2{1.0f, 1.0f}));
  glUniform2fv(shader::kDynamicSpriteTranslate, 1, glm::value_ptr(translate_));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

UiSlots::UiSlots(const std::size_t& total_size, UiButton&& btn_next,
                 UiButton&& btn_prev, UiButton&& slot1,
                 UiButton&& slot2, UiButton&& slot3,
                 UiButton&& slot4, UiButton&& slot5,
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

void UiSlots::Render(const Shader& shader) const {
//  shader_.SetUniform("brightness", 1.0f);
  btn_next_.Render();
  btn_prev_.Render();
  //TODO: maybe we should pass shader to ctor?
  RenderSlot(shader, slot1_, 0);
  RenderSlot(shader, slot2_, 1);
  RenderSlot(shader, slot3_, 2);
  RenderSlot(shader, slot4_, 3);
  RenderSlot(shader, slot5_, 4);
}

void UiSlots::RenderPicking(const Shader& shader) const {
  btn_next_.RenderPicking(shader);
  btn_prev_.RenderPicking(shader);
  slot1_.RenderPicking(shader);
  slot2_.RenderPicking(shader);
  slot3_.RenderPicking(shader);
  slot4_.RenderPicking(shader);
  slot5_.RenderPicking(shader);
}

bool UiSlots::Press(std::uint32_t id) {
  //TODO: bvh
  if (id == slot1_.GetId()) {
    edit_mode_selected_sample_id_ = start_idx_ + 0;
  } else if (id == slot2_.GetId()) {
    edit_mode_selected_sample_id_ = start_idx_ + 1;
  } else if (id == slot3_.GetId()) {
    edit_mode_selected_sample_id_ = start_idx_ + 2;
  } else if (id == slot4_.GetId()) {
    edit_mode_selected_sample_id_ = start_idx_ + 3;
  } else if (id == slot5_.GetId()) {
    edit_mode_selected_sample_id_ = start_idx_ + 4;
  } else if (id == btn_next_.GetId()) {
    if (start_idx_ + 5 < total_size_) {
      ++start_idx_;
    }
  } else if (id == btn_prev_.GetId()) {
    if (start_idx_ > 0) {
      --start_idx_;
    }
  } else {
    return false;
  }
  return true;
}

int UiSlots::Hover(std::uint32_t id) const {
  //TODO: bvh
  if (id == btn_next_.GetId()) {
    return btn_next_.Hover();
  } else if (id == btn_prev_.GetId()) {
    return btn_prev_.Hover();
  } else if (id == slot1_.GetId()) {
    return slot1_.Hover();
  } else if (id == slot2_.GetId()) {
    return slot2_.Hover();
  } else if (id == slot3_.GetId()) {
    return slot3_.Hover();
  } else if (id == slot4_.GetId()) {
    return slot4_.Hover();
  } else if (id == slot5_.GetId()) {
    return slot5_.Hover();
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

void UiSlots::RenderSlot(
    const Shader& shader, const UiButton& slot, int i) const {
  if (start_idx_ + i == edit_mode_selected_sample_id_) {
    glUniform1f(shader::kSpriteBrightness, 0.5f);
  } else {
    glUniform1f(shader::kSpriteBrightness, 1.0f);
  }
  if (total_size_ - 1 > start_idx_ + i) {
    int pos_in_slots = static_cast<int>(i & colors_.size());
    glUniform4fv(shader::kSpriteColor, 1,
                 glm::value_ptr(colors_[pos_in_slots]));
  } else {
    glUniform4fv(shader::kSpriteColor, 1, glm::value_ptr(colors::kBlack));
  }
  slot.Render();
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

UiSlider::UiSlider(
    UiButton&& min_handle, UiButton&& max_handle,
    UiButton&& track, UiSliderHandle&& handle)
    : min_handle_(min_handle),
      max_handle_(max_handle),
      track_(track),
      handle_(handle) {
  height_ = track_.GetTopBorder() - track_.GetBottomBorder();
}

void UiSlider::Render(const Shader& sprite_shader, const Shader& slider_shader) const {
  sprite_shader.Bind();
  min_handle_.Render();
  max_handle_.Render();
  track_.Render();
  slider_shader.Bind();
  handle_.Render(slider_shader);
}
void UiSlider::RenderPicking(const Shader& picking_shader) const {
  track_.RenderPicking(picking_shader);
  // there's no point in rendering picking for the rest nested components
}

int UiSlider::Hover(std::uint32_t id) const {
  //TODO: bvh
  if (id == min_handle_.GetId()) {
    return min_handle_.Hover();
  } else if (id == max_handle_.GetId()) {
    return max_handle_.Hover();
  } else if (id == track_.GetId()) {
    return track_.Hover();
  } else if (id == handle_.GetId()) {
    return handle_.Hover();
  }
  return {};
}

void UiSlider::UpdateSliderPos(glm::vec2 position) {
  float new_y_pos = std::clamp(position.y, track_.GetBottomBorder(),
                               track_.GetTopBorder());
  progress_ = (new_y_pos - track_.GetBottomBorder()) / height_;
  handle_.SetPositionOffset((progress_ - 1) * height_);
}

std::uint32_t UiSlider::GetTrackId() const {
  return track_.GetId();
}
