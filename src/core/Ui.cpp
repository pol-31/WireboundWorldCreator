#include "Ui.h"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "../modes/TerrainMode.h"
#include "../common/Colors.h"

[[nodiscard]] bool UiButton::AabbContains(glm::vec2 position) const {
  return position.x >= GetLeftBorder() && position.x <= GetRightBorder() &&
         position.y <= GetTopBorder() && position.y >= GetBottomBorder();
}

void UiButton::Render() const {
  shader_.Bind();
  glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset_, 4);
}

// see src/vbos/UiDataMain.h for explanation
float UiButton::GetLeftBorder() const {
  return details::kUiVboDataMain[vbo_offset_ + 8];
}

float UiButton::GetRightBorder() const {
  return details::kUiVboDataMain[vbo_offset_];
}

float UiButton::GetTopBorder() const {
  return details::kUiVboDataMain[vbo_offset_ + 5];
}

float UiButton::GetBottomBorder() const {
  return details::kUiVboDataMain[vbo_offset_ + 1];
}

void UiButtonTerrainModeBake::Press(glm::vec2 position [[maybe_unused]]) {
  terrain_mode_.Bake();
}

void UiButtonTerrainModeSmooth::Press(glm::vec2 position [[maybe_unused]]) {
  terrain_mode_.SwitchSmooth();
}

void UiSliderHandle::Render() const {
  shader_.Bind();
  shader_.SetUniform("pos_y_down_offset_", pos_y_down_offset_);
  glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset_, 4);
}

UiSlots::UiSlots(const Shader& shader, std::string_view description,
                 PointDataType points_data, UiStaticSprite& btn_next,
                 UiStaticSprite& btn_prev, UiStaticSprite& slot1,
                 UiStaticSprite& slot2, UiStaticSprite& slot3,
                 UiStaticSprite& slot4, UiStaticSprite& slot5,
                 int& edit_mode_selected_sample_id)
    : IUi(shader, description),
      points_data_(points_data),
      btn_next_(btn_next),
      btn_prev_(btn_prev),
      slot1_(slot1),
      slot2_(slot2),
      slot3_(slot3),
      slot4_(slot4),
      slot5_(slot5),
      edit_mode_selected_sample_id_(edit_mode_selected_sample_id) {
  InitAabb();
  InitColors();
}

bool UiSlots::AabbContains(glm::vec2 position) const {
  return position.x >= aabb_.left && position.x <= aabb_.right &&
         position.y <= aabb_.top && position.y >= aabb_.bottom;
}

void UiSlots::Render() const {
  shader_.SetUniform("brightness", 1.0f);
  btn_next_.Render();
  btn_prev_.Render();

  //TODO: smells a little bit...
  RenderSlot(slot1_, 0);
  RenderSlot(slot2_, 1);
  RenderSlot(slot3_, 2);
  RenderSlot(slot4_, 3);
  RenderSlot(slot5_, 4);

  //TODO: render points
}

//TODO: binary search / bvh
void UiSlots::Press(glm::vec2 position) {
  if (slot1_.AabbContains(position)) {
    edit_mode_selected_sample_id_ = start_idx_ + 0;
  } else if (slot2_.AabbContains(position)) {
    edit_mode_selected_sample_id_ = start_idx_ + 1;
  } else if (slot3_.AabbContains(position)) {
    edit_mode_selected_sample_id_ = start_idx_ + 2;
  } else if (slot4_.AabbContains(position)) {
    edit_mode_selected_sample_id_ = start_idx_ + 3;
  } else if (slot5_.AabbContains(position)) {
    edit_mode_selected_sample_id_ = start_idx_ + 4;
  } else if (btn_next_.AabbContains(position)) {
    if (start_idx_ + 5 < points_data_.size()) {
      ++start_idx_;
    }
  } else if (btn_prev_.AabbContains(position)) {
    if (start_idx_ > 0) {
      --start_idx_;
    }
  }
}

//TODO: binary search / bvh
std::string_view UiSlots::Hover(glm::vec2 position) const {
  if (slot1_.AabbContains(position)) {
    return slot1_.Hover(position);
  } else if (slot2_.AabbContains(position)) {
    return slot2_.Hover(position);
  } else if (slot3_.AabbContains(position)) {
    return slot3_.Hover(position);
  } else if (slot4_.AabbContains(position)) {
    return slot4_.Hover(position);
  } else if (slot5_.AabbContains(position)) {
    return slot5_.Hover(position);
  } else if (btn_next_.AabbContains(position)) {
    return btn_next_.Hover(position);
  } else if (btn_prev_.AabbContains(position)) {
    return btn_prev_.Hover(position);
  } else {
    return {};
  }
}

void UiSlots::InitAabb() {
  std::array<float, 7> borders{
      btn_next_.GetLeftBorder(), btn_prev_.GetLeftBorder(),
      slot1_.GetLeftBorder(), slot2_.GetLeftBorder(),
      slot3_.GetLeftBorder(), slot4_.GetLeftBorder(),
      slot5_.GetLeftBorder()
  };
  aabb_.left = *std::min(borders.begin(), borders.end());
  borders = {
      btn_next_.GetRightBorder(), btn_prev_.GetRightBorder(),
      slot1_.GetRightBorder(), slot2_.GetRightBorder(),
      slot3_.GetRightBorder(), slot4_.GetRightBorder(),
      slot5_.GetRightBorder()
  };
  aabb_.right = *std::max(borders.begin(), borders.end());
  borders = {
      btn_next_.GetTopBorder(), btn_prev_.GetTopBorder(),
      slot1_.GetTopBorder(), slot2_.GetTopBorder(),
      slot3_.GetTopBorder(), slot4_.GetTopBorder(),
      slot5_.GetTopBorder()
  };
  aabb_.top = *std::max(borders.begin(), borders.end());
  borders = {
      btn_next_.GetBottomBorder(), btn_prev_.GetBottomBorder(),
      slot1_.GetBottomBorder(), slot2_.GetBottomBorder(),
      slot3_.GetBottomBorder(), slot4_.GetBottomBorder(),
      slot5_.GetBottomBorder()
  };
  aabb_.bottom = *std::min(borders.begin(), borders.end());
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

void UiSlots::RenderSlot(UiStaticSprite& slot, int i) const {
  if (start_idx_ + i == edit_mode_selected_sample_id_) {
    shader_.SetUniform("brightness", 0.5f);
  } else {
    shader_.SetUniform("brightness", 1.0f);
  }
  if (points_data_.size() - 1 > start_idx_ + i) {
    int pos_in_slots = i & colors_.size();
    shader_.SetUniformVec4(
        "color", 1, glm::value_ptr(colors_[pos_in_slots]));
  } else {
    shader_.SetUniformVec4(
        "color", 1, glm::value_ptr(colors::kBlack));
  }
  slot.Render();
}

UiBiomesList::UiBiomesList(
    const Shader& shader, std::string_view description,
    int vbo_offset_tex_cell, int vbo_offset_tex_all,
    int total_num, int vbo_pos_offset, int instances_num)
    : IUi(shader, description) {}

//TODO; binary search again
void UiBiomesList::Press(glm::vec2 position) {
  //TODO: return instanced id or -1
  //  affects cur_page_offset_
  //TODO: check like binary search (from the centre) - we can allow ourselves to do so
}

void UiBiomesList::Render() const {
  //TODO:
  // render non-existing cells (tex - black)
  // render non-selected cells (brightness - 0.5f)
  // render selected cell (brightness 1.0f)
  //TODO: render points
}

std::string_view UiBiomesList::Hover(glm::vec2 position) const {
  //TODO:
}

UiSlider::UiSlider(
    const Shader& shader, std::string_view description,
    UiStaticSprite&& min_handle, UiStaticSprite&& max_handle,
    UiStaticSprite&& track, UiSliderHandle&& handle)
    : IUi(shader, description),
      min_handle_(min_handle),
      max_handle_(max_handle),
      track_(track),
      handle_(handle) {
  InitAabb();
}

bool UiSlider::AabbContains(glm::vec2 position) const {
  return position.x >= aabb_.left && position.x <= aabb_.right &&
         position.y <= aabb_.top && position.y >= aabb_.bottom;
}

void UiSlider::Render() const {
  min_handle_.Render();
  max_handle_.Render();
  track_.Render();
  handle_.Render();
}

std::string_view UiSlider::Hover(glm::vec2 position) const {
  if (min_handle_.AabbContains(position)) {
    return min_handle_.Hover(position);
  } else if (max_handle_.AabbContains(position)) {
    return max_handle_.Hover(position);
  } else if (track_.AabbContains(position)) {
    return track_.Hover(position);
  } else if (handle_.AabbContains(position)) {
    return handle_.Hover(position);
  } else {
    return {};
  }
}

void UiSlider::Press(glm::vec2 position) {
  std::cout << "we don't use it" << std::endl;
  /*if (!TrackAreaAabbContains(position)) {
    return;
  }
  progress_ = (position.y - track_.GetBottomBorder()) / height_;
  handle_.SetPositionOffset(progress_, -height_ * progress_);*/
}

void UiSlider::UpdateSliderPos(glm::vec2 position) {
  float new_y_pos = std::clamp(position.y, track_.GetBottomBorder(),
                               track_.GetTopBorder());
  progress_ = (new_y_pos - track_.GetBottomBorder()) / height_;
  handle_.SetPositionOffset(progress_, -height_ * progress_);
}

void UiSlider::InitAabb() {
  std::array<float, 4> borders{
      min_handle_.GetLeftBorder(), max_handle_.GetLeftBorder(),
      track_.GetLeftBorder(), handle_.GetLeftBorder()
  };
  aabb_.left = *std::min(borders.begin(), borders.end());
  borders = {
      min_handle_.GetRightBorder(), max_handle_.GetRightBorder(),
      track_.GetRightBorder(), handle_.GetRightBorder()
  };
  aabb_.right = *std::max(borders.begin(), borders.end());
  borders = {
      min_handle_.GetTopBorder(), max_handle_.GetTopBorder(),
      track_.GetTopBorder(), handle_.GetTopBorder()
  };
  aabb_.top = *std::max(borders.begin(), borders.end());
  borders = {
      min_handle_.GetBottomBorder(), max_handle_.GetBottomBorder(),
      track_.GetBottomBorder(), handle_.GetBottomBorder()
  };
  aabb_.bottom = *std::min(borders.begin(), borders.end());

  height_ = track_.GetTopBorder() - track_.GetBottomBorder();
}

bool UiSlider::TrackAreaAabbContains(glm::vec2 position) {
  return position.x >= track_.GetLeftBorder() &&
         position.x <= track_.GetRightBorder() &&
         position.y <= track_.GetTopBorder() &&
         position.y >= track_.GetBottomBorder();
}

void BvhNode::Render() const {
  left_child_->Render();
  right_child_->Render();
}

void BvhNode::Press(glm::vec2 position) {
  if (left_child_->AabbContains(position)) {
    left_child_->Press(position);
  } else if (right_child_->AabbContains(position)) {
    right_child_->Press(position);
  }
}

std::string_view BvhNode::Hover(glm::vec2 position) const {
  if (left_child_->AabbContains(position)) {
    return left_child_->Hover(position);
  } else if (right_child_->AabbContains(position)) {
    return right_child_->Hover(position);
  } else {
    return {};
  }
}

//TODO: each Edit Mode stores this rather that std::vector
//  as an acceleration structure for Press() and Hover()
BvhNode* GenBvhHierarchy(std::vector<IUi*> components) {
  // sah by Y-coord?
}
