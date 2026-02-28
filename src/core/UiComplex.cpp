#include "UiComplex.h"

#include <format>

#include "../common/Details.h"
#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"
#include "../common/models/ModelLoader.h"
#include "../common/models/ModelManager.h"
#include "../io/Camera.h"

UiWindowSlider::UiWindowSlider(UiSprite&& sp_track, UiSprite&& sp_handle,
                               int slots_num, float track_length_factor,
                               float slots_length_factor)
    : UiBase(sp_track.GetId(), {}),
      sp_track_(std::move(sp_track)),
      sp_handle_(std::move(sp_handle)),
      slots_num_(slots_num),
      track_length_factor_(track_length_factor),
      slots_length_factor_(slots_length_factor),
      length_(track_length_factor *
              (sp_track_.GetTopBorder() - sp_track_.GetBottomBorder())),
      length_slots_(slots_length_factor *
                    (sp_track_.GetTopBorder() - sp_track_.GetBottomBorder())),
      centre_((sp_track_.GetTopBorder() + sp_track_.GetBottomBorder()) / 2.0f) {
  gUiComponents[sp_handle_.GetId() - details::kIdOffsetUi].parent_id_ =
      sp_track_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
  UpdateTransform();
}

void UiWindowSlider::SetParentTransform(LocalTransform transform) {
  sp_track_.SetParentTransform(transform);
  sp_handle_.SetParentTransform(transform);
  UpdateTransform();
}

void UiWindowSlider::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  sp_track_.Render();
  sp_handle_.Render();
}

void UiWindowSlider::SetUpScissors() const {
  glScissor(0, scissors_start_, gWindowWidth, scissors_length_);
}

void UiWindowSlider::RenderPicking() const {
  sp_track_.RenderPicking();
  if (debug::gUiAltMode) {
    sp_handle_.RenderPicking();
  }
}

void UiWindowSlider::Press() { pressed_ = true; }

void UiWindowSlider::Release() { pressed_ = false; }

void UiWindowSlider::SetValue(float value) { Set(value); }

bool UiWindowSlider::Scroll(GLuint id, float yoffset) {
  //  if (id > sp_handle_.GetId() || id < sp_track_.GetId()) {
  //    return false;
  //  }
  //  Set(progress_ + yoffset * 0.01f);
  //  return true;
  return false;
}

// hierarchy, ui_event_handler; slot's-content...

void UiWindowSlider::SetTranslate(glm::vec2 translate) {
  sp_track_.SetTranslate(translate);
  sp_handle_.SetTranslate(translate);
}

float UiWindowSlider::GetProgress() const { return progress_; }

void UiWindowSlider::UpdateTransform() {
  length_ = track_length_factor_ *
            (sp_track_.GetTopBorder() - sp_track_.GetBottomBorder());
  length_slots_ = slots_length_factor_ *
                  (sp_track_.GetTopBorder() - sp_track_.GetBottomBorder());
  centre_ = (sp_track_.GetTopBorder() + sp_track_.GetBottomBorder()) / 2.0f;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({0.0f, related_pos});
  sp_track_.UpdateTransform();
  sp_handle_.UpdateTransform();
}

void UiWindowSlider::SetEntryNum(int entry_num) {
  entry_num_ = entry_num;
  Set(1.0f);
  UpdateRenderData();
}

void UiWindowSlider::UpdateRenderData() {
#ifndef NDEBUG
  if (!sp_slot_) {
    //    throw "no sp_slot_, unable to calculate slot_height";
    return;
  }
#endif
  // total n, visible n or n - 1
  slot_height_ = slots_length_factor_ *
                 (sp_slot_->GetTopBorder() - sp_slot_->GetBottomBorder());
  auto scrollable_slots =
      static_cast<float>(std::max(entry_num_ - slots_num_ + 1, 0));
  // issue at progress_==1. so ui_slots::Render() call rener entry_num+1 times
  float float_index = scrollable_slots * progress_ * 0.99f;
  cur_slots_offset_ = (int)float_index;
  float fractional_part = float_index - cur_slots_offset_;
  float offset_y = fractional_part * slot_height_;
  start_slot_translate_ = glm::vec2{0.0f, offset_y};

  float y_ndc = centre_ - length_ / 2;
  scissors_start_ = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
  scissors_length_ = int(length_ * 0.5f * gWindowHeight);
}

int UiWindowSlider::GetSlotId(glm::vec2 mouse_pos) {
  float half_slot_height = slot_height_ / 2.0f;
  float border = centre_ + length_slots_ / 2.0f - 1.5 * slot_height_ +
                 start_slot_translate_.y;
  for (int i = 0; i < slots_num_ - 1; ++i) {
    if (mouse_pos.y > border) {
      return i + cur_slots_offset_;
    }
    border -= slot_height_;
  }
  return slots_num_ - 1 + cur_slots_offset_;  // else cond
}

void UiWindowSlider::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_slots_ / 2.0f;
  float offset =
      glm::clamp(mouse_pos.y - centre_, -half_length_, +half_length_);
  progress_ = 1.0 - (offset + half_length_) / length_slots_;
  glm::vec2 translate = {0.0f, offset};
  sp_handle_.SetTranslate(translate);
  UpdateRenderData();
}

void UiWindowSlider::Set(float progress) {
  // track length unscaled: need just top_border - bottom_border
  progress_ = progress;
  float half_length_ = length_slots_ / 2.0f;
  float offset = -((progress_ - 1.0f) * length_slots_) - half_length_;
  glm::vec2 translate = {0.0f, offset};
  sp_handle_.SetTranslate(translate);
  UpdateRenderData();
}

void UiWindowSlider::FocusOnSelected(int slot_id) {
  if (slot_id == -1 || entry_num_ < slots_num_) {
    return;
  }
  float fractional_part;
  if (slot_id - cur_slots_offset_ == 0) {
    fractional_part = 0.0f;
  } else if (slot_id - cur_slots_offset_ == slots_num_ - 1) {
    fractional_part = 0.99f;
  } else {
    return;
  }
  float float_index = fractional_part + cur_slots_offset_;
  auto scrollable_slots =
      static_cast<float>(std::max(entry_num_ - slots_num_ + 1, 0));
  std::cout << "was " << progress_;
  progress_ = float_index / scrollable_slots;
  std::cout << " become " << progress_ << std::endl;
  Set(progress_);
}

UiLoading::UiLoading(UiSprite&& sprite0, UiSprite&& sprite10,
                     UiSprite&& sprite20, UiSprite&& sprite30,
                     UiSprite&& sprite40, UiSprite&& sprite50,
                     UiSprite&& sprite60, UiSprite&& sprite70,
                     UiSprite&& sprite80, UiSprite&& sprite90,
                     UiSprite&& sprite100)
    : sprites_{{std::move(sprite0), std::move(sprite10), std::move(sprite20),
                std::move(sprite30), std::move(sprite40), std::move(sprite50),
                std::move(sprite60), std::move(sprite70), std::move(sprite80),
                std::move(sprite90), std::move(sprite100)}},
      hierarchy_(&sprites_[0],
                 {&sprites_[1], &sprites_[2], &sprites_[3], &sprites_[4],
                  &sprites_[5], &sprites_[6], &sprites_[7], &sprites_[8],
                  &sprites_[9], &sprites_[10]}) {}

void UiLoading::Render(float progress) {
  int idx = static_cast<int>(std::round(progress * 10.0f));
  idx = std::clamp(idx, 0, 10);
  sprites_[idx].Render();
}

void UiLoading::RenderPicking() const { sprites_[0].RenderPicking(); }

UiSelectedSprite::UiSelectedSprite(UiSprite&& sp_selected,
                                   const UiSprite* sp_ptr_top)
    : sp_selected_(std::move(sp_selected)), sp_ptr_top_(sp_ptr_top) {
  SetSelected(0);
}

void UiSelectedSprite::Render() { sp_selected_.Render(); }

void UiSelectedSprite::SetSelected(int id) {
  auto centre = sp_ptr_top_->GetCentre();
  centre.y -= id * sp_ptr_top_->GetHeight();
  sp_selected_.SetTranslate(centre);
}
