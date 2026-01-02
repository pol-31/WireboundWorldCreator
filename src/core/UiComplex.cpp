#include "UiComplex.h"

#include <format>

#include "../common/Details.h"
#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"
#include "../common/models/ModelLoader.h"
#include "../common/models/ModelManager.h"
#include "../io/Camera.h"

UiWindowSlider::UiWindowSlider(UiDynamicSprite&& sp_track,
                               UiDynamicSprite&& sp_handle, int slots_num,
                               float track_length_factor,
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

UiWindowSlider::UiWindowSlider(UiWindowSlider&& other) noexcept
    : UiBase(std::move(other)),
      sp_track_(std::move(other.sp_track_)),
      sp_handle_(std::move(other.sp_handle_)),
      track_length_factor_(other.track_length_factor_),
      slots_length_factor_(other.slots_length_factor_),
      slots_num_(other.slots_num_),
      entry_num_(other.entry_num_),
      slot_(other.slot_),
      centre_(other.centre_),
      length_(other.length_),
      length_slots_(other.length_slots_),
      progress_(other.progress_),
      pressed_(other.pressed_),
      slot_height_(other.slot_height_),
      scissors_start_(other.scissors_start_),
      scissors_length_(other.scissors_length_),
      cur_slots_offset_(other.cur_slots_offset_),
      start_slot_translate_(other.start_slot_translate_) {
  gUiComponents[sp_track_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
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

// ---
// ---
// ---

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
  float float_index = scrollable_slots * progress_;
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

UiLoading::UiLoading(UiDynamicSprite&& sprite0, UiDynamicSprite&& sprite10,
                     UiDynamicSprite&& sprite20, UiDynamicSprite&& sprite30,
                     UiDynamicSprite&& sprite40, UiDynamicSprite&& sprite50,
                     UiDynamicSprite&& sprite60, UiDynamicSprite&& sprite70,
                     UiDynamicSprite&& sprite80, UiDynamicSprite&& sprite90,
                     UiDynamicSprite&& sprite100)
    : sprites_{{std::move(sprite0), std::move(sprite10), std::move(sprite20),
                std::move(sprite30), std::move(sprite40), std::move(sprite50),
                std::move(sprite60), std::move(sprite70), std::move(sprite80),
                std::move(sprite90), std::move(sprite100)}},
      hierarchy_(&sprites_[0]) {
  hierarchy_ =
      UiHierarchy(&sprites_[0], &sprites_[1], &sprites_[2], &sprites_[3],
                  &sprites_[4], &sprites_[5], &sprites_[6], &sprites_[7],
                  &sprites_[8], &sprites_[9], &sprites_[10]);
}

UiLoading::UiLoading(UiLoading&& other) noexcept
    : sprites_(std::move(other.sprites_)),
      hierarchy_(std::move(other.hierarchy_)) {
  hierarchy_ =
      UiHierarchy(&sprites_[0], &sprites_[1], &sprites_[2], &sprites_[3],
                  &sprites_[4], &sprites_[5], &sprites_[6], &sprites_[7],
                  &sprites_[8], &sprites_[9], &sprites_[10]);
}

void UiLoading::Render(float progress) {
  int idx = static_cast<int>(std::round(progress * 10.0f));
  idx = std::clamp(idx, 0, 10);
  sprites_[idx].Render();
}

void UiLoading::RenderPicking() const { sprites_[0].RenderPicking(); }

UiSelectedSprite::UiSelectedSprite(UiDynamicSprite&& sp_selected,
                                   const UiDynamicSprite* sp_ptr_top)
    : sp_selected_(std::move(sp_selected)), sp_ptr_top_(sp_ptr_top) {
  SetSelected(0);
}

UiSelectedSprite::UiSelectedSprite(UiSelectedSprite&& other) noexcept
    : sp_selected_(std::move(other.sp_selected_)),
      sp_ptr_top_(other.sp_ptr_top_) {
  SetSelected(0);
}

void UiSelectedSprite::Render() { sp_selected_.Render(); }

void UiSelectedSprite::SetSelected(int id) {
  auto centre = sp_ptr_top_->GetCentre();
  centre.y -= id * sp_ptr_top_->GetHeight();
  sp_selected_.SetTranslate(centre);
}

UiCompass::UiCompass(const Camera* camera, UiDynamicSprite&& sp_compass,
                     UiDynamicSprite&& sp_north, UiDynamicSprite&& sp_south,
                     UiDynamicSprite&& sp_east, UiDynamicSprite&& sp_west)
    : camera_(camera),
      sp_compass_(std::move(sp_compass)),
      sp_north_(std::move(sp_north)),
      sp_south_(std::move(sp_south)),
      sp_east_(std::move(sp_east)),
      sp_west_(std::move(sp_west)),
      hierarchy_(&sp_compass_) {
  hierarchy_ =
      UiHierarchy(&sp_compass_, &sp_north_, &sp_south_, &sp_east_, &sp_west_);
}

UiCompass::UiCompass(UiCompass&& other) noexcept
    : camera_(other.camera_),
      sp_compass_(std::move(other.sp_compass_)),
      sp_north_(std::move(other.sp_north_)),
      sp_south_(std::move(other.sp_south_)),
      sp_east_(std::move(other.sp_east_)),
      sp_west_(std::move(other.sp_west_)),
      hierarchy_(std::move(other.hierarchy_)) {
  hierarchy_ =
      UiHierarchy(&sp_compass_, &sp_north_, &sp_south_, &sp_east_, &sp_west_);
}

void UiCompass::Render() {
  float factor =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  glm::vec2 radius = glm::vec2(0.05f, 0.05f * factor);
  float yaw = glm::radians(camera_->GetYaw());
  LocalTransform transform;

  transform.rotate = yaw;
  sp_compass_.SetParentTransform(transform);
  transform.rotate = 0.0f;

  // North (yaw)
  transform.translate = glm::vec2(radius.x * cos(yaw), radius.y * sin(yaw));
  sp_north_.SetParentTransform(transform);

  // South (yaw + 180°)
  transform.translate = glm::vec2(radius.x * cos(yaw + glm::pi<float>()),
                                  radius.y * sin(yaw + glm::pi<float>()));
  sp_south_.SetParentTransform(transform);

  // East (yaw + 90°)
  transform.translate = glm::vec2(radius.x * cos(yaw + glm::half_pi<float>()),
                                  radius.y * sin(yaw + glm::half_pi<float>()));
  sp_east_.SetParentTransform(transform);

  // West (yaw - 90°)
  transform.translate = glm::vec2(radius.x * cos(yaw - glm::half_pi<float>()),
                                  radius.y * sin(yaw - glm::half_pi<float>()));
  sp_west_.SetParentTransform(transform);

  sp_compass_.Render();
  sp_north_.Render();
  sp_south_.Render();
  sp_east_.Render();
  sp_west_.Render();
}

void UiCompass::RenderPicking() const { sp_compass_.RenderPicking(); }

UiTopWindowBase::UiTopWindowBase(UiDynamicSprite&& desk, float size_scale,
                                 UiSharedResources& ui_shared_resources,
                                 WindowQueue& window_queue)
    : desk_(std::move(desk)),
      size_scale_(size_scale),
      ui_shared_resources_(ui_shared_resources),
      window_queue_(window_queue),
      hierarchy_(&desk_) {
  desk_.SetScale(size_scale);
}

UiTopWindowBase::UiTopWindowBase(UiTopWindowBase&& other) noexcept
    : desk_(std::move(other.desk_)),
      size_scale_(other.size_scale_),
      ui_shared_resources_(other.ui_shared_resources_),
      window_queue_(other.window_queue_),
      hierarchy_(std::move(other.hierarchy_)) {}

void UiTopWindowBase::Show() { window_queue_.SetTopWindow(this); }

void UiTopWindowBase::Hide() { window_queue_.SetTopWindow(nullptr); }

UiCaution::UiCaution(UiDynamicSprite&& desk, float size_scale,
                     UiSharedResources& ui_shared_resources,
                     WindowQueue& window_queue, UiDynamicSprite&& text)
    : UiTopWindowBase(std::move(desk), size_scale, ui_shared_resources,
                      window_queue),
      text_(std::move(text)) {
  hierarchy_ = UiHierarchy(&desk_, &text_);
}

UiCaution::UiCaution(UiCaution&& other) noexcept
    : Base(std::move(other)), text_(std::move(other.text_)) {
  hierarchy_ = UiHierarchy(&desk_, &text_);
}

bool UiCaution::Render() {
  ui_shared_resources_.shader_sp_.Bind();
  desk_.Render();
  //  text_.Render();
  return false;
}

void UiCaution::RenderPicking() {
  ui_shared_resources_.shader_sp_picking_.Bind();
  desk_.RenderPicking();
  //  text_.RenderPicking();
}

void UiCaution::Press(int id) { Hide(); }

void UiCaution::Release() {}

UiConfirmation::UiConfirmation(UiDynamicSprite&& desk, float size_scale,
                               UiSharedResources& ui_shared_resources,
                               WindowQueue& window_queue,
                               UiDynamicSprite&& btn_accept,
                               UiDynamicSprite&& btn_decline,
                               UiTextMenuId&& text)
    : UiTopWindowBase(std::move(desk), size_scale, ui_shared_resources,
                      window_queue),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      text_(std::move(text)),
      ui_event_handler_({&btn_accept_, &btn_decline_}) {
  hierarchy_ = UiHierarchy(&desk_, &btn_accept_, &btn_decline_, &text_);
}

UiConfirmation::UiConfirmation(UiConfirmation&& other) noexcept
    : Base(std::move(other)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      text_(std::move(other.text_)),
      ui_event_handler_({&btn_accept_, &btn_decline_}) {
  hierarchy_ = UiHierarchy(&desk_, &btn_accept_, &btn_decline_, &text_);
}

bool UiConfirmation::Render() {
  ui_shared_resources_.shader_sp_.Bind();
  desk_.Render();
  btn_accept_.Render();
  btn_decline_.Render();
  text_.Render();
  return false;
}

void UiConfirmation::RenderPicking() {
  ui_shared_resources_.shader_sp_picking_.Bind();
  desk_.RenderPicking();
  btn_accept_.RenderPicking();
  btn_decline_.RenderPicking();
  text_.RenderPicking();
}

void UiConfirmation::Press(int id) {
  //  ui_event_handler_.Press(id);
  if (id == btn_accept_.GetId()) {
    callable_();  // TODO: but... why not bth_accept_.Press()... should do smt
  } else if (id != desk_.GetId()) {
    Hide();
  }
}

void UiConfirmation::Release() { ui_event_handler_.Release(); }

void UiConfirmation::SetText(data::TextId text_id) { text_.SetText(text_id); }

void UiConfirmation::SetCallable(std::function<void()>&& callable) {
  callable_ = std::move(callable);
}

void UiConfirmation::BtnEnter() { callable_(); }

void UiConfirmation::BtnEscape() {
  btn_decline_.Press();
  Hide();
}

UiFile::UiFile(UiDynamicSprite&& desk, float size_scale,
               UiSharedResources& ui_shared_resources,
               WindowQueue& window_queue, UiDynamicSprite&& btn_accept,
               UiDynamicSprite&& btn_decline, UiText&& label,
               UiTextInput&& text)
    : UiTopWindowBase(std::move(desk), size_scale, ui_shared_resources,
                      window_queue),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      label_(std::move(label)),
      text_(std::move(text)),
      ui_event_handler_({&btn_accept_ /*, &btn_decline_*/}) {
  hierarchy_ =
      UiHierarchy(&desk_, &btn_accept_, &btn_decline_, &label_, &text_);
  label_.SetText("label");
}

UiFile::UiFile(UiFile&& other) noexcept
    : Base(std::move(other)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      label_(std::move(other.label_)),
      text_(std::move(other.text_)),
      ui_event_handler_({&btn_accept_ /*, &btn_decline_*/}) {
  hierarchy_ =
      UiHierarchy(&desk_, &btn_accept_, &btn_decline_, &label_, &text_);
}

bool UiFile::Render() {
  ui_shared_resources_.shader_sp_.Bind();
  desk_.Render();
  btn_accept_.Render();
  btn_decline_.Render();
  label_.Render();
  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  text_.Render();
  return false;
}

void UiFile::RenderPicking() {
  ui_shared_resources_.shader_sp_picking_.Bind();
  desk_.RenderPicking();
  btn_accept_.RenderPicking();
  btn_decline_.RenderPicking();
  label_.RenderPicking();
  ui_shared_resources_.shader_sp_picking_.Bind();
  text_.RenderPicking();
}

void UiFile::Press(int id) {
  ui_event_handler_.Press(id);
  if (id == text_.GetId()) {
    text_.Press();
    return;
  }
  if (id == btn_accept_.GetId() || id == btn_decline_.GetId()) {
    Hide();
  }
}

void UiFile::Release() { ui_event_handler_.Release(); }

UiWindowBase::UiWindowBase(UiDynamicSprite&& sprite, float size_scale,
                           UiToggle2&& pin,
                           UiSharedResources& ui_shared_resources,
                           WindowQueue& window_queue)
    : sprite_(std::move(sprite)),
      size_scale_(size_scale),
      pin_(std::move(pin)),
      ui_shared_resources_(ui_shared_resources),
      window_queue_(window_queue),
      speed_(4.0f),
      hierarchy_(&sprite_) {
  sprite_.SetScale(size_scale);
}

UiWindowBase::UiWindowBase(UiWindowBase&& other) noexcept
    : sprite_(std::move(other.sprite_)),
      size_scale_(other.size_scale_),
      pin_(std::move(other.pin_)),
      ui_shared_resources_(other.ui_shared_resources_),
      window_queue_(other.window_queue_),
      speed_(other.speed_),
      progress_(other.progress_),
      back_ready_(other.back_ready_),
      hierarchy_(std::move(other.hierarchy_)) {}

UiWindowBase* UiWindowBase::GetWindowPtr() { return this; }

// TODO: useless?
void UiWindowBase::Show() {
  do_show_ = true;
  if (window_queue_id_ != -1) {
    return;
  }
  window_queue_id_ = window_queue_.PushBack(GetWindowPtr());
}

// unpin -> if(do_show) Hide()

void UiWindowBase::Hide() {
  do_show_ = false;
  if (window_queue_id_ == -1 || Pinned()) {
    return;
  }
  window_queue_.Erase(window_queue_id_);
  window_queue_id_ = -1;
  progress_ = 0.0f;
}

void UiWindowBase::ForceHide() {
  do_show_ = false;
  if (window_queue_id_ == -1) {
    return;
  }
  pin_.Set(false);
  window_queue_.Erase(window_queue_id_);
  window_queue_id_ = -1;
  progress_ = 0.0f;
}

bool UiWindowBase::Pinned() const noexcept { return pin_.TurnedOn(); }

UiWindowAppear::UiWindowAppear(UiDynamicSprite&& sprite, float size_scale,
                               UiToggle2&& pin,
                               UiSharedResources& ui_shared_resources,
                               WindowQueue& window_queue)
    : UiWindowBase(std::move(sprite), size_scale, std::move(pin),
                   ui_shared_resources, window_queue) {}

UiWindowAppear::UiWindowAppear(UiWindowAppear&& other) noexcept
    : UiWindowBase(std::move(other)) {}

/// back_ready_==false when appearing or disappearing animation
/// returs false when disappearing fading is over
bool UiWindowAppear::RenderBack(bool show) {
  if (show) {
    progress_ += speed_ * gDeltaTime;
    if (progress_ >= 1.0f) {
      back_ready_ = true;
    }
  } else {
    back_ready_ = false;
    progress_ -= speed_ * gDeltaTime;
    if (progress_ <= 0.0f) {
      return false;
    }
  }
  progress_ = std::clamp(progress_, 0.0f, 1.0f);
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glActiveTexture(GL_TEXTURE1);
  ui_shared_resources_.shader_sp_mask_.Bind();
  ui_shared_resources_.tex_ui_mask_.Bind();
  glUniform1f(4, progress_);
  sprite_.Render();
  ui_shared_resources_.shader_sp_.Bind();
  glActiveTexture(GL_TEXTURE0);  // go back to default
  pin_.Render();
  return true;
}

void UiWindowAppear::RenderPickingBack() {
  ui_shared_resources_.shader_sp_picking_.Bind();
  sprite_.RenderPicking();
  pin_.RenderPicking();
}

UiObjectInfo::UiObjectInfo(
    UiDynamicSprite&& sprite, float size_scale, UiToggle2&& pin,
    UiSharedResources& ui_shared_resources, WindowQueue& window_queue,

    UiDynamicSprite&& sp_enemy, UiDynamicSprite&& sp_friend,
    UiDynamicSprite&& sp_neutral, UiDynamicSprite&& sp_obstacle,
    UiText&& txt_name, UiText&& txt_characteristic, UiText&& txt_value)
    : UiWindowAppear(std::move(sprite), size_scale, std::move(pin),
                     ui_shared_resources, window_queue),
      sp_enemy_(std::move(sp_enemy)),
      sp_friend_(std::move(sp_friend)),
      sp_neutral_(std::move(sp_neutral)),
      sp_obstacle_(std::move(sp_obstacle)),
      txt_name_(std::move(txt_name)),
      txt_characteristic_(std::move(txt_characteristic)),
      txt_value_(std::move(txt_value)),
      ui_event_handler_({&pin_}) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &sp_enemy_, &sp_friend_, &sp_neutral_,
                  &sp_obstacle_, &txt_name_, &txt_characteristic_, &txt_value_);
  speed_ = 2.0f;
}

UiObjectInfo::UiObjectInfo(UiObjectInfo&& other) noexcept
    : Base(std::move(other)),
      sp_enemy_(std::move(other.sp_enemy_)),
      sp_friend_(std::move(other.sp_friend_)),
      sp_neutral_(std::move(other.sp_neutral_)),
      sp_obstacle_(std::move(other.sp_obstacle_)),
      txt_name_(std::move(other.txt_name_)),
      txt_characteristic_(std::move(other.txt_characteristic_)),
      txt_value_(std::move(other.txt_value_)),
      ui_event_handler_({&pin_}) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &sp_enemy_, &sp_friend_, &sp_neutral_,
                  &sp_obstacle_, &txt_name_, &txt_characteristic_, &txt_value_);
}

bool UiObjectInfo::Render() {
  ui_shared_resources_.tex_ui_.Bind();
  bool stop_show = Base::RenderBack(true);
  //  if (!Base::BackIsReady()) {
  //    return stop_show;
  //  }

  ui_shared_resources_.shader_sp_.Bind();
  glUniform1f(1, 0.4f);  /// half-saturated color for non-selected
  sp_enemy_.Render();
  sp_friend_.Render();
  sp_neutral_.Render();
  sp_obstacle_.Render();
  glUniform1f(1, 1.0f);  /// full saturated color for selected
  if (enemy_selected_) {
    sp_enemy_.Render();
  }
  if (friend_selected_) {
    sp_friend_.Render();
  }
  if (neutral_selected_) {
    sp_neutral_.Render();
  }
  if (obstacle_selected_) {
    sp_obstacle_.Render();
  }
  txt_name_.Render();
  RenderParams();
  return stop_show;
}

void UiObjectInfo::RenderParams() {
  float entry_height =
      sprite_.GetHeight() / (render_params_.size() + 5);  // +pads
  LocalTransform transform;
  for (const auto& p : render_params_) {
    txt_characteristic_.SetParentTransform(transform);
    txt_characteristic_.SetText(p.name);
    txt_characteristic_.Render();
    txt_value_.SetParentTransform(transform);
    txt_value_.SetText(std::format("{:.2f}", p.value));
    txt_value_.Render();
    transform.translate.y -= entry_height;
  }
}

void UiObjectInfo::RenderPickingParams() {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height =
      sprite_.GetHeight() / (render_params_.size() + 5);  // +pads
  LocalTransform transform;
  for (const auto& p : render_params_) {
    txt_characteristic_.SetParentTransform(transform);
    txt_characteristic_.SetText(p.name);
    txt_characteristic_.RenderPicking();
    txt_value_.SetParentTransform(transform);
    txt_value_.SetText(std::to_string(p.value));
    txt_value_.RenderPicking();
    transform.translate -= entry_height;
  }
}

void UiObjectInfo::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  ui_shared_resources_.shader_sp_picking_.Bind();
  sp_enemy_.RenderPicking();
  sp_friend_.RenderPicking();
  sp_neutral_.RenderPicking();
  sp_obstacle_.RenderPicking();
  txt_name_.RenderPicking();
  RenderPickingParams();
}

bool UiObjectInfo::Press(int id) {
  bool handled = ui_event_handler_.Press(id);
  if (id != pin_.GetId()) {
    return handled;
  }
  if (!do_show_) {
    Hide();
  }
  return handled;
}

void UiObjectInfo::Release() { ui_event_handler_.Release(); }

void UiObjectInfo::Show(bool enemy_selected, bool friend_selected,
                        bool neutral_selected, bool obstacle_selected,
                        int selected_num) {
  enemy_selected_ = enemy_selected;
  friend_selected_ = friend_selected;
  neutral_selected_ = neutral_selected;
  obstacle_selected_ = obstacle_selected;
  txt_name_.SetText("Pavlushas (a lot)");
  selected_num_ = selected_num;
  render_params_.clear();
  render_params_.emplace_back("amount", selected_num);
  Show();
}

void UiObjectInfo::Show(bool enemy_selected, bool friend_selected,
                        bool neutral_selected, bool obstacle_selected,
                        const ModelData* mdl_data) {
  enemy_selected_ = enemy_selected;
  friend_selected_ = friend_selected;
  neutral_selected_ = neutral_selected;
  obstacle_selected_ = obstacle_selected;
  txt_name_.SetText("Pavlusha");
  selected_num_ = 1;
  mdl_data_ = mdl_data;
  render_params_.clear();
  render_params_.emplace_back("hp", mdl_data_->hp);
  render_params_.emplace_back("speed", mdl_data_->speed);
  render_params_.emplace_back("attack", mdl_data_->attack);
  render_params_.emplace_back("attack speed", mdl_data_->attack_speed);
  Show();
}

UiTipWindow::UiTipWindow(UiDynamicSprite&& sprite, float size_scale,
                         UiToggle2&& pin,
                         UiSharedResources& ui_shared_resources,
                         WindowQueue& window_queue, UiDynamicSprite&& text)
    : UiWindowAppear(std::move(sprite), size_scale, std::move(pin),
                     ui_shared_resources, window_queue),
      text_(std::move(text)) {                        // 1
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &text_);  // 2
  speed_ = 2.0f;
}

UiTipWindow::UiTipWindow(UiTipWindow&& other) noexcept
    : Base(std::move(other)), text_(std::move(other.text_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &text_);
}

bool UiTipWindow::Render() {
  UpdateHoverState(ui_shared_resources_.gltf_context_.hovered_id);
  bool stop_show = Base::RenderBack(hovered_);
  if (!Base::BackIsReady()) {
    return stop_show;
  }
  ui_shared_resources_.shader_sp_.Bind();
  text_.Render();
  return stop_show;
}

void UiTipWindow::RenderPicking() { Base::RenderPickingBack(); }

bool UiTipWindow::Press(int id) {
  if (id == sprite_.GetId()) {
    sprite_.Press();
  } else if (id == pin_.GetId()) {
    pin_.Press();
  } else {
    return false;
  }
  if (!do_show_) {
    Hide();
  }
  return false;
}

void UiTipWindow::Release() {}

void UiTipWindow::UpdateHoverState(int id) {
  hovered_ = (id >= sprite_.GetId() && id <= text_.GetId());
}

void UiTipWindow::SetText(data::TextId text_id) {
  std::cout << "text id is " << static_cast<int>(text_id);
}

UiWindowPopUp::UiWindowPopUp(UiDynamicSprite&& sprite, float size_scale,
                             UiToggle2&& pin,
                             UiSharedResources& ui_shared_resources,
                             WindowQueue& window_queue,
                             LocalTransform start_transform,
                             LocalTransform end_transform)
    : UiWindowBase(std::move(sprite), size_scale, std::move(pin),
                   ui_shared_resources, window_queue),
      start_transform_(start_transform),
      end_transform_(end_transform) {}

UiWindowPopUp::UiWindowPopUp(UiWindowPopUp&& other) noexcept
    : UiWindowBase(std::move(other)),
      cur_transform_(other.cur_transform_),
      start_transform_(other.start_transform_),
      end_transform_(other.end_transform_) {}

bool UiWindowPopUp::RenderBack(bool show) {
  ui_shared_resources_.shader_sp_.Bind();
  if (pin_.TurnedOn() || show) {
    progress_ += speed_ * gDeltaTime;
    if (progress_ >= 1.0f) {
      back_ready_ = true;
    }
  } else {
    progress_ -= speed_ * gDeltaTime;
  }
  progress_ = std::clamp(progress_, 0.0f, 1.0f);
  CubicInterpolation();
  sprite_.SetParentTransform(cur_transform_);
  pin_.SetParentTransform(cur_transform_);
  sprite_.Render();
  pin_.Render();
  if (progress_ == 0.0f) {
    return false;
  } else {
    return true;
  }
}

void UiWindowPopUp::RenderPickingBack() {
  ui_shared_resources_.shader_sp_picking_.Bind();
  sprite_.RenderPicking();
  pin_.RenderPicking();
}

void UiWindowPopUp::CubicInterpolation() {
  cur_transform_.translate =
      start_transform_.translate +
      progress_ * (end_transform_.translate - start_transform_.translate);
  cur_transform_.rotate =
      start_transform_.rotate +
      progress_ * (end_transform_.rotate - start_transform_.rotate);
  cur_transform_.scale =
      start_transform_.scale +
      progress_ * (end_transform_.scale - start_transform_.scale);
}

UiSettings::UiSettings(
    UiDynamicSprite&& sprite, float size_scale, UiToggle2&& pin,
    UiSharedResources& ui_shared_resources, WindowQueue& window_queue,
    LocalTransform start_transform, LocalTransform end_transform,
    UiDynamicSprite&& resolution_label, UiDynamicSprite&& resolution_left,
    UiDynamicSprite&& resolution_right, UiDynamicSprite&& resolution,
    UiToggle4&& toggle_fullscreen, UiSliderH2 sensitivity, UiSliderH2&& sound,
    UiToggle4&& toggle_sound, UiSliderH2&& music, UiToggle4&& toggle_music,
    UiDynamicSprite&& tip_info_label, UiDynamicSprite&& tip_info,
    UiToggle4&& toggle_tip_info)
    : UiWindowPopUp(std::move(sprite), size_scale, std::move(pin),
                    ui_shared_resources, window_queue, start_transform,
                    end_transform),
      resolution_label_(std::move(resolution_label)),
      resolution_left_(std::move(resolution_left)),
      resolution_right_(std::move(resolution_right)),
      resolution_(std::move(resolution)),
      toggle_fullscreen_(std::move(toggle_fullscreen)),
      sensitivity_(std::move(sensitivity)),
      sound_(std::move(sound)),
      toggle_sound_(std::move(toggle_sound)),
      music_(std::move(music)),
      toggle_music_(std::move(toggle_music)),
      tip_info_label_(std::move(tip_info_label)),
      tip_info_(std::move(tip_info)),
      toggle_tip_info_(std::move(toggle_tip_info)),
      ui_event_handler_({&pin_, &resolution_left_, &resolution_right_,
                         &toggle_fullscreen_, &sensitivity_, &sound_,
                         &toggle_sound_, &music_, &toggle_music_, &tip_info_,
                         &toggle_tip_info_}) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &resolution_label_,
                           &resolution_left_, &resolution_right_, &resolution_,
                           &toggle_fullscreen_, &sensitivity_, &sound_,
                           &toggle_sound_, &music_, &toggle_music_,
                           &tip_info_label_, &tip_info_, &toggle_tip_info_);
}

UiSettings::UiSettings(UiSettings&& other) noexcept
    : Base(std::move(other)),
      resolution_label_(std::move(other.resolution_label_)),
      resolution_left_(std::move(other.resolution_left_)),
      resolution_right_(std::move(other.resolution_right_)),
      resolution_(std::move(other.resolution_)),
      toggle_fullscreen_(std::move(other.toggle_fullscreen_)),
      sensitivity_(std::move(other.sensitivity_)),
      sound_(std::move(other.sound_)),
      toggle_sound_(std::move(other.toggle_sound_)),
      music_(std::move(other.music_)),
      toggle_music_(std::move(other.toggle_music_)),
      tip_info_label_(std::move(other.tip_info_label_)),
      tip_info_(std::move(other.tip_info_)),
      toggle_tip_info_(std::move(other.toggle_tip_info_)),
      ui_event_handler_({&pin_, &resolution_left_, &resolution_right_,
                         &toggle_fullscreen_, &sensitivity_, &sound_,
                         &toggle_sound_, &music_, &toggle_music_, &tip_info_,
                         &toggle_tip_info_}) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &resolution_label_,
                           &resolution_left_, &resolution_right_, &resolution_,
                           &toggle_fullscreen_, &sensitivity_, &sound_,
                           &toggle_sound_, &music_, &toggle_music_,
                           &tip_info_label_, &tip_info_, &toggle_tip_info_);
}

// returns "stop render"
bool UiSettings::Render() {
  UpdateHoverState(ui_shared_resources_.gltf_context_.hovered_id);
  ui_shared_resources_.tex_ui_.Bind();
  bool stop_show = Base::RenderBack(hovered_);
  //  if (!Base::BackIsReady()) {
  //    return stop_show;
  //  }
  ui_shared_resources_.shader_sp_.Bind();

  resolution_label_.SetParentTransform(Base::cur_transform_);
  resolution_left_.SetParentTransform(Base::cur_transform_);
  resolution_right_.SetParentTransform(Base::cur_transform_);
  resolution_.SetParentTransform(Base::cur_transform_);
  toggle_fullscreen_.SetParentTransform(Base::cur_transform_);
  sensitivity_.SetParentTransform(Base::cur_transform_);
  sound_.SetParentTransform(Base::cur_transform_);
  toggle_sound_.SetParentTransform(Base::cur_transform_);
  music_.SetParentTransform(Base::cur_transform_);
  toggle_music_.SetParentTransform(Base::cur_transform_);
  tip_info_label_.SetParentTransform(Base::cur_transform_);
  tip_info_.SetParentTransform(Base::cur_transform_);
  toggle_tip_info_.SetParentTransform(Base::cur_transform_);

  auto mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_;

  resolution_left_.Render();
  resolution_right_.Render();
  resolution_.Render();
  toggle_fullscreen_.Render();
  sensitivity_.Render(mouse_pos);
  sound_.Render(mouse_pos);
  toggle_sound_.Render();
  music_.Render(mouse_pos);
  toggle_music_.Render();
  tip_info_.Render();
  toggle_tip_info_.Render();

  ui_shared_resources_.shader_sp_.Bind();

  ui_shared_resources_.gltf_context_.text_renderer->RenderText(
      resolution_label_, "resolution", 0.05f, glm::vec2{0.0f});
  ui_shared_resources_.gltf_context_.text_renderer->RenderText(
      tip_info_label_, "show tips", 0.05f, glm::vec2{0.0f});

  return stop_show;
}

void UiSettings::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  ui_shared_resources_.shader_sp_picking_.Bind();

  resolution_left_.RenderPicking();
  resolution_right_.RenderPicking();
  resolution_.RenderPicking();
  toggle_fullscreen_.RenderPicking();
  sensitivity_.RenderPicking();
  sound_.RenderPicking();
  toggle_sound_.RenderPicking();
  music_.RenderPicking();
  toggle_music_.RenderPicking();
  tip_info_.RenderPicking();
  toggle_tip_info_.RenderPicking();

  ui_shared_resources_.shader_sp_picking_.Bind();

  ui_shared_resources_.gltf_context_.text_renderer->RenderTextPicking(
      resolution_label_, "resolution", 0.05f, glm::vec2{0.0f});
  ui_shared_resources_.gltf_context_.text_renderer->RenderTextPicking(
      tip_info_label_, "show tips", 0.05f, glm::vec2{0.0f});
}

void UiSettings::UpdateHoverState(int id) {
  hovered_ = (id >= sprite_.GetId() && id <= toggle_tip_info_.GetId());
}

bool UiSettings::Press(int id) {
  bool handled = ui_event_handler_.Press(id);
  if (id != pin_.GetId()) {
    return handled;
  }
  if (!do_show_) {
    Hide();
  }
  return handled;
}

void UiSettings::Release() { ui_event_handler_.Release(); }

bool UiSettings::Scroll(GLuint id, float yoffset) {
  return music_.Scroll(id, yoffset) || sound_.Scroll(id, yoffset) ||
         sensitivity_.Scroll(id, yoffset);
}

UiPlayerMap::UiPlayerMap(UiSharedResources& ui_shared_resources,
                         WindowQueue& window_queue)
    : UiWindowPopUp(
          {data::VboIdMain::kPlayerPhoneMap}, 1.0f,
          {{data::VboIdMain::kMapFlowerRed}, {data::VboIdMain::kMapFlowerBlue}},
          ui_shared_resources, window_queue,
          LocalTransform{glm::vec2{0.2f, 0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f}),
      sp_player_(data::VboIdMain::kPlayerMapPlayer),
      sp_enemy_(data::VboIdMain::kPlayerMapEnemy),
      sp_friend_(data::VboIdMain::kPlayerMapFriend),
      sp_neutral_(data::VboIdMain::kPlayerMapNeutral),
      sp_obstacle_(data::VboIdMain::kPlayerMapObstacle) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &sp_player_, &sp_enemy_,
                           &sp_friend_, &sp_neutral_, &sp_obstacle_);
}

UiPlayerMap::UiPlayerMap(UiPlayerMap&& other) noexcept
    : Base(std::move(other)),
      sp_player_(std::move(other.sp_player_)),
      sp_enemy_(std::move(other.sp_enemy_)),
      sp_friend_(std::move(other.sp_friend_)),
      sp_neutral_(std::move(other.sp_neutral_)),
      sp_obstacle_(std::move(other.sp_obstacle_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &sp_player_, &sp_enemy_,
                           &sp_friend_, &sp_neutral_, &sp_obstacle_);
}

// TODO: remove? but how...
bool UiPlayerMap::Render() {
  std::cerr << "used wrong UiPlayerMap::Render() func" << std::endl;
  return false;
}

void UiPlayerMap::Render(ModelManager* mdl_manager) {
  UpdateHoverState(ui_shared_resources_.gltf_context_.hovered_id);
  ui_shared_resources_.tex_ui_.Bind();
  bool stop_show = Base::RenderBack(hovered_);
  ui_shared_resources_.shader_sp_.Bind();

  sp_player_.SetParentTransform(Base::cur_transform_);
  sp_enemy_.SetParentTransform(Base::cur_transform_);
  sp_friend_.SetParentTransform(Base::cur_transform_);
  sp_neutral_.SetParentTransform(Base::cur_transform_);
  sp_obstacle_.SetParentTransform(Base::cur_transform_);

  glEnable(GL_SCISSOR_TEST);
  float centre_height =
      (sprite_.GetTopBorder() + sprite_.GetBottomBorder()) / 2.0f + 0.025f;
  float length_y = sprite_.GetHeight() / 2.3f;
  float y_ndc = centre_height - length_y / 2;
  int y_px = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
  int height_py = int(length_y * 0.5f * gWindowHeight);

  float centre_width =
      (sprite_.GetRightBorder() + sprite_.GetLeftBorder()) / 2.0f;
  float length_x = sprite_.GetWidth() * 0.84f;
  float x_ndc = centre_width - length_x / 2;
  int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
  int width_px = int(length_x * 0.5f * gWindowWidth);

  glScissor(x_px, y_px, width_px, height_py);
  mdl_manager->RenderOnMap(&sp_player_, &sp_enemy_, &sp_friend_, &sp_neutral_,
                           &sp_obstacle_);
  glDisable(GL_SCISSOR_TEST);
}

void UiPlayerMap::RenderPicking() { Base::RenderPickingBack(); }

void UiPlayerMap::UpdateHoverState(int id) {
  hovered_ = (id >= sprite_.GetId() && id <= sp_obstacle_.GetId());
}

bool UiPlayerMap::Press(int id) {
  if (id != pin_.GetId()) {
    return false;
  }
  if (!do_show_) {
    Hide();
  }
  return false;
}

void UiPlayerMap::Release() {}

bool UiPlayerMap::Scroll(GLuint id, float yoffset) { return false; }
