#include "UiComplex.h"

#include "../common/UiDebugger.h"
#include "../common/ShadersBinding.h"
#include "../common/TextRenderer.h"
#include "../common/Details.h"
#include "../io/Cameras.h"

UiWindowSlider::UiWindowSlider(
    UiDynamicSprite&& sp_track,
    UiDynamicSprite&& sp_handle,
    int slots_num,
    float track_length_factor,
    float slots_length_factor)
    : UiBase(sp_track.GetId(), {}),
      sp_track_(std::move(sp_track)),
      sp_handle_(std::move(sp_handle)),
      slots_num_(slots_num),
      track_length_factor_(track_length_factor),
      slots_length_factor_(slots_length_factor),
      length_(track_length_factor
              * (sp_track_.GetTopBorder() - sp_track_.GetBottomBorder())),
      length_slots_(slots_length_factor
                    * (sp_track_.GetTopBorder() - sp_track_.GetBottomBorder())),
      centre_((sp_track_.GetTopBorder() + sp_track_.GetBottomBorder()) / 2.0f) {
  gUiComponents[sp_handle_.GetId() - details::kIdOffsetUi].parent_id_
      = sp_track_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
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
  gUiComponents[sp_track_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiWindowSlider::SetParentTransform(LocalTransform transform) {
  sp_track_.SetParentTransform(transform);
  sp_handle_.SetParentTransform(transform);
}

void UiWindowSlider::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    std::cout << entry_num_ << std::endl;
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

void UiWindowSlider::Press() {
  pressed_ = true;
}

void UiWindowSlider::Release() {
  pressed_ = false;
}

void UiWindowSlider::SetValue(float value) {
  Set(value);
}

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

float UiWindowSlider::GetProgress() const {
  return progress_;
}

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
  auto scrollable_slots = static_cast<float>(std::max(entry_num_ - slots_num_ + 1, 0));
  float float_index = scrollable_slots * progress_;
  cur_slots_offset_ = (int)float_index;
  float fractional_part = float_index - cur_slots_offset_;
  float offset_y = fractional_part * slot_height_;
  start_slot_translate_ = glm::vec2{0.0f, -0.201f + offset_y};

  float y_ndc = centre_ - length_ / 2;
  scissors_start_ = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
  scissors_length_ = int(length_ * 0.5f * gWindowHeight);
}

int UiWindowSlider::GetSlotId(glm::vec2 mouse_pos) {
  float half_slot_height = slot_height_ / 2.0f;
  float border = centre_ + length_slots_ / 2.0f + half_slot_height + start_slot_translate_.y;
  for (int i = 0; i < slots_num_ - 1; ++i) {
    if (mouse_pos.y > border) {
      return i + cur_slots_offset_;
    }
    border -= slot_height_;
  }
  return slots_num_ - 1 + cur_slots_offset_; // else cond
}

void UiWindowSlider::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_slots_ / 2.0f;
  float offset = glm::clamp(
      mouse_pos.y - centre_, -half_length_, +half_length_);
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
  auto scrollable_slots = static_cast<float>(std::max(entry_num_ - slots_num_ + 1, 0));
  std::cout << "was " << progress_;
  progress_ = float_index / scrollable_slots;
  std::cout << " become " << progress_ << std::endl;
  Set(progress_);
}

UiLoading::UiLoading(
    UiStaticSprite&& sprite0,
    UiStaticSprite&& sprite10,
    UiStaticSprite&& sprite20,
    UiStaticSprite&& sprite30,
    UiStaticSprite&& sprite40,
    UiStaticSprite&& sprite50,
    UiStaticSprite&& sprite60,
    UiStaticSprite&& sprite70,
    UiStaticSprite&& sprite80,
    UiStaticSprite&& sprite90,
    UiStaticSprite&& sprite100)
    : sprites_{
          {std::move(sprite0), std::move(sprite10), std::move(sprite20),
           std::move(sprite30), std::move(sprite40), std::move(sprite50),
           std::move(sprite60), std::move(sprite70), std::move(sprite80),
           std::move(sprite90), std::move(sprite100)}},
      hierarchy_(&sprites_[0]) {
  hierarchy_ = UiHierarchy(
      &sprites_[0], &sprites_[1], &sprites_[2], &sprites_[3],
      &sprites_[4], &sprites_[5], &sprites_[6], &sprites_[7],
      &sprites_[8], &sprites_[9], &sprites_[10]);
}

UiLoading::UiLoading(UiLoading&& other) noexcept
    : sprites_(std::move(other.sprites_)),
      hierarchy_(std::move(other.hierarchy_)) {
  hierarchy_ = UiHierarchy(
      &sprites_[0], &sprites_[1], &sprites_[2], &sprites_[3],
      &sprites_[4], &sprites_[5], &sprites_[6], &sprites_[7],
      &sprites_[8], &sprites_[9], &sprites_[10]);
}

void UiLoading::Render(float progress) {
  int idx = static_cast<int>(std::round(progress * 10.0f));
  idx = std::clamp(idx, 0, 10);
  sprites_[idx].Render();
}

void UiLoading::RenderPicking() const {
  sprites_[0].RenderPicking();
}

UiCompass::UiCompass(
    const CameraHandler* camera,
    UiDynamicSprite&& sp_compass,
    UiDynamicSprite&& sp_north,
    UiDynamicSprite&& sp_south,
    UiDynamicSprite&& sp_east,
    UiDynamicSprite&& sp_west)
    : camera_(camera),
      sp_compass_(std::move(sp_compass)),
      sp_north_(std::move(sp_north)),
      sp_south_(std::move(sp_south)),
      sp_east_(std::move(sp_east)),
      sp_west_(std::move(sp_west)),
      hierarchy_(&sp_compass_) {
  hierarchy_ = UiHierarchy(
      &sp_compass_, &sp_north_, &sp_south_, &sp_east_, &sp_west_);
}

UiCompass::UiCompass(UiCompass&& other) noexcept
    : camera_(other.camera_),
      sp_compass_(std::move(other.sp_compass_)),
      sp_north_(std::move(other.sp_north_)),
      sp_south_(std::move(other.sp_south_)),
      sp_east_(std::move(other.sp_east_)),
      sp_west_(std::move(other.sp_west_)),
      hierarchy_(std::move(other.hierarchy_)) {
  hierarchy_ = UiHierarchy(
      &sp_compass_, &sp_north_, &sp_south_, &sp_east_, &sp_west_);
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
  transform.translate = glm::vec2(
      radius.x * cos(yaw),
      radius.y * sin(yaw)
  );
  sp_north_.SetParentTransform(transform);

  // South (yaw + 180°)
  transform.translate = glm::vec2(
      radius.x * cos(yaw + glm::pi<float>()),
      radius.y * sin(yaw + glm::pi<float>())
  );
  sp_south_.SetParentTransform(transform);

  // East (yaw + 90°)
  transform.translate = glm::vec2(
      radius.x * cos(yaw + glm::half_pi<float>()),
      radius.y * sin(yaw + glm::half_pi<float>())
  );
  sp_east_.SetParentTransform(transform);

  // West (yaw - 90°)
  transform.translate = glm::vec2(
      radius.x * cos(yaw - glm::half_pi<float>()),
      radius.y * sin(yaw - glm::half_pi<float>())
  );
  sp_west_.SetParentTransform(transform);

  sp_compass_.Render();
  sp_north_.Render();
  sp_south_.Render();
  sp_east_.Render();
  sp_west_.Render();
}

void UiCompass::RenderPicking() const {
  sp_compass_.RenderPicking();
}

UiTopWindowBase::UiTopWindowBase(
    UiDynamicSprite&& desk,
    float size_scale,
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

void UiTopWindowBase::Show() {
  window_queue_.SetTopWindow(this);
}

void UiTopWindowBase::Hide() {
  window_queue_.SetTopWindow(nullptr);
}

UiCaution::UiCaution(
    UiDynamicSprite&& desk,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    UiDynamicSprite&& text)
    : UiTopWindowBase(std::move(desk), size_scale,
                      ui_shared_resources, window_queue),
      text_(std::move(text)) {
  hierarchy_ = UiHierarchy(&desk_, &text_);
}

UiCaution::UiCaution(UiCaution&& other) noexcept
    : Base(std::move(other)),
      text_(std::move(other.text_)) {
  hierarchy_ = UiHierarchy(&desk_, &text_);
}

bool UiCaution::Render() {
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  desk_.Render();
  //  text_.Render();
  return false;
}

void UiCaution::RenderPicking() {
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  desk_.RenderPicking();
  //  text_.RenderPicking();
}

void UiCaution::Press(int id) {
  Hide();
}

void UiCaution::Release() {}

UiConfirmation::UiConfirmation(
    UiDynamicSprite&& desk,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    UiDynamicSprite&& btn_accept,
    UiDynamicSprite&& btn_decline,
    UiTextMenuId&& text)
    : UiTopWindowBase(std::move(desk), size_scale,
                      ui_shared_resources, window_queue),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      text_(std::move(text)),
      ui_event_handler_({&btn_accept_, &btn_decline_}) {
  hierarchy_ = UiHierarchy(
      &desk_, &btn_accept_, &btn_decline_, &text_);
}

UiConfirmation::UiConfirmation(UiConfirmation&& other) noexcept
    : Base(std::move(other)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      text_(std::move(other.text_)),
      ui_event_handler_({&btn_accept_, &btn_decline_}) {
  hierarchy_ = UiHierarchy(
      &desk_, &btn_accept_, &btn_decline_, &text_);
}

bool UiConfirmation::Render() {
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  desk_.Render();
  btn_accept_.Render();
  btn_decline_.Render();
  text_.Render();
  return false;
}

void UiConfirmation::RenderPicking() {
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  desk_.RenderPicking();
  btn_accept_.RenderPicking();
  btn_decline_.RenderPicking();
  text_.RenderPicking();
}

void UiConfirmation::Press(int id) {
//  ui_event_handler_.Press(id);
  if (id == btn_accept_.GetId()) {
    callable_(); // TODO: but... why not bth_accept_.Press()... should do smt
  } else if (id != desk_.GetId()) {
    Hide();
  }
}

void UiConfirmation::Release() {
  ui_event_handler_.Release();
}

void UiConfirmation::SetText(data::TextId text_id) {
  text_.SetText(text_id);
}

void UiConfirmation::SetCallable(std::function<void()>&& callable) {
  callable_ = std::move(callable);
}

void UiConfirmation::BtnEnter() {
  callable_();
}

void UiConfirmation::BtnEscape() {
  btn_decline_.Press();
  Hide();
}

UiFile::UiFile(
    UiDynamicSprite&& desk,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    UiDynamicSprite&& btn_accept,
    UiDynamicSprite&& btn_decline, UiText&& label,
    UiTextInput&& text)
    : UiTopWindowBase(std::move(desk), size_scale,
                      ui_shared_resources, window_queue),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      label_(std::move(label)),
      text_(std::move(text)),
      ui_event_handler_({&btn_accept_/*, &btn_decline_*/}) {
  hierarchy_ = UiHierarchy(
      &desk_, &btn_accept_, &btn_decline_, &label_, &text_);
  label_.SetText("label");
}

UiFile::UiFile(UiFile&& other) noexcept
    : Base(std::move(other)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      label_(std::move(other.label_)),
      text_(std::move(other.text_)),
      ui_event_handler_({&btn_accept_/*, &btn_decline_*/}) {
  hierarchy_ = UiHierarchy(
      &desk_, &btn_accept_, &btn_decline_, &label_, &text_);
}

bool UiFile::Render() {
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  desk_.Render();
  btn_accept_.Render();
  btn_decline_.Render();
  label_.Render();
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  text_.Render();
  return false;
}

void UiFile::RenderPicking() {
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  desk_.RenderPicking();
  btn_accept_.RenderPicking();
  btn_decline_.RenderPicking();
  label_.RenderPicking();
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
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

void UiFile::Release() {
  ui_event_handler_.Release();
}

UiWindowBase::UiWindowBase(
    UiDynamicSprite&& sprite,
    float size_scale,
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

UiWindowBase* UiWindowBase::GetWindowPtr() {
  return this;
}

//TODO: useless?
void UiWindowBase::Show() {
  do_show_ = true;
  if (window_queue_id_ != -1) {
    return;
  }
  window_queue_id_ = window_queue_.PushBack(GetWindowPtr());
}

//unpin -> if(do_show) Hide()

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

bool UiWindowBase::Pinned() const noexcept {
  return pin_.TurnedOn();
}

UiWindowAppear::UiWindowAppear(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue)
    : UiWindowBase(std::move(sprite), size_scale,
                   std::move(pin), ui_shared_resources,
                   window_queue) {}

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
  ui_shared_resources_.mask_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_mask_.Bind();
  glUniform1f(shader::kSpriteProgress, progress_);
  sprite_.Render();
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  glActiveTexture(GL_TEXTURE0); // go back to default
  pin_.Render();
  return true;
}

void UiWindowAppear::RenderPickingBack() {
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sprite_.RenderPicking();
  pin_.RenderPicking();
}

UiTabMenu::UiTabMenu(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,

    UiStaticSprite&& btn_mode_terrain,
    UiStaticSprite&& btn_mode_water,
    UiStaticSprite&& btn_mode_roads,
    UiStaticSprite&& btn_mode_fences,
    UiStaticSprite&& btn_mode_placement,
    UiStaticSprite&& btn_mode_objects,
    UiStaticSprite&& btn_mode_biomes,
    UiStaticSprite&& btn_mode_tiles,

    UiToggle4&& toggle_terrain, UiToggle4&& toggle_water,
    UiToggle4&& toggle_roads, UiToggle4&& toggle_fences,
    UiToggle4&& toggle_placement, UiToggle4&& toggle_objects,
    UiToggle4&& toggle_biomes, UiToggle4&& toggle_tiles,

    UiStaticSprite&& btn_shader_wirebound, UiToggle4&& toggle_shaders,

    UiDynamicSprite&& arrow_select,
    UiDynamicSprite&& arrow_selected,
    UiDynamicSprite&& save_data,
    UiDynamicSprite&& load_data)
    : UiWindowAppear(std::move(sprite), size_scale,
                     std::move(pin), ui_shared_resources,
                     window_queue),
      btn_mode_terrain_(std::move(btn_mode_terrain)),
      btn_mode_water_(std::move(btn_mode_water)),
      btn_mode_roads_(std::move(btn_mode_roads)),
      btn_mode_fences_(std::move(btn_mode_fences)),
      btn_mode_placement_(std::move(btn_mode_placement)),
      btn_mode_objects_(std::move(btn_mode_objects)),
      btn_mode_biomes_(std::move(btn_mode_biomes)),
      btn_mode_tiles_(std::move(btn_mode_tiles)),

      toggle_terrain_(std::move(toggle_terrain)),
      toggle_water_(std::move(toggle_water)),
      toggle_roads_(std::move(toggle_roads)),
      toggle_fences_(std::move(toggle_fences)),
      toggle_placement_(std::move(toggle_placement)),
      toggle_objects_(std::move(toggle_objects)),
      toggle_biomes_(std::move(toggle_biomes)),
      toggle_tiles_(std::move(toggle_tiles)),

      btn_shader_wirebound_(std::move(btn_shader_wirebound)),
      toggle_shaders_(std::move(toggle_shaders)),

      arrow_select_(std::move(arrow_select)),
      arrow_selected_(std::move(arrow_selected)),
      save_data_(std::move(save_data)),
      load_data_(std::move(load_data)),
      ui_event_handler_({
          &pin_,
          &btn_mode_terrain_,
          &btn_mode_water_,
          &btn_mode_roads_,
          &btn_mode_fences_,
          &btn_mode_placement_,
          &btn_mode_objects_,
          &btn_mode_biomes_,
          &btn_mode_tiles_,

          &toggle_terrain_,
          &toggle_water_,
          &toggle_roads_,
          &toggle_fences_,
          &toggle_placement_,
          &toggle_objects_,
          &toggle_biomes_,
          &toggle_tiles_,

          &btn_shader_wirebound_,
          &toggle_shaders_,
          &save_data_,
          &load_data_,
      }) {
  hierarchy_ = UiHierarchy(
      &sprite_, &pin_, &btn_mode_terrain_, &btn_mode_water_,
      &btn_mode_roads_, &btn_mode_fences_, &btn_mode_placement_,
      &btn_mode_objects_, &btn_mode_biomes_, &btn_mode_tiles_,
      &toggle_terrain_, &toggle_water_, &toggle_roads_, &toggle_fences_,
      &toggle_placement_, &toggle_objects_, &toggle_biomes_, &toggle_tiles_,
      &btn_shader_wirebound_, &toggle_shaders_, &arrow_select_, &arrow_selected_,
      &save_data_, &load_data_);
  speed_ = 2.0f;
}

UiTabMenu::UiTabMenu(UiTabMenu&& other) noexcept
    : Base(std::move(other)),
      btn_mode_terrain_(std::move(other.btn_mode_terrain_)),
      btn_mode_water_(std::move(other.btn_mode_water_)),
      btn_mode_roads_(std::move(other.btn_mode_roads_)),
      btn_mode_fences_(std::move(other.btn_mode_fences_)),
      btn_mode_placement_(std::move(other.btn_mode_placement_)),
      btn_mode_objects_(std::move(other.btn_mode_objects_)),
      btn_mode_biomes_(std::move(other.btn_mode_biomes_)),
      btn_mode_tiles_(std::move(other.btn_mode_tiles_)),

      toggle_terrain_(std::move(other.toggle_terrain_)),
      toggle_water_(std::move(other.toggle_water_)),
      toggle_roads_(std::move(other.toggle_roads_)),
      toggle_fences_(std::move(other.toggle_fences_)),
      toggle_placement_(std::move(other.toggle_placement_)),
      toggle_objects_(std::move(other.toggle_objects_)),
      toggle_biomes_(std::move(other.toggle_biomes_)),
      toggle_tiles_(std::move(other.toggle_tiles_)),

      btn_shader_wirebound_(std::move(other.btn_shader_wirebound_)),
      toggle_shaders_(std::move(other.toggle_shaders_)),

      arrow_select_(std::move(other.arrow_select_)),
      arrow_selected_(std::move(other.arrow_selected_)),
      save_data_(std::move(other.save_data_)),
      load_data_(std::move(other.load_data_)),

      ui_event_handler_({
          &pin_,
          &btn_mode_terrain_,
          &btn_mode_water_,
          &btn_mode_roads_,
          &btn_mode_fences_,
          &btn_mode_placement_,
          &btn_mode_objects_,
          &btn_mode_biomes_,
          &btn_mode_tiles_,

          &toggle_terrain_,
          &toggle_water_,
          &toggle_roads_,
          &toggle_fences_,
          &toggle_placement_,
          &toggle_objects_,
          &toggle_biomes_,
          &toggle_tiles_,

          &btn_shader_wirebound_,
          &toggle_shaders_,
          &save_data_,
          &load_data_,
      }) {
  hierarchy_ = UiHierarchy(
      &sprite_, &pin_, &btn_mode_terrain_, &btn_mode_water_,
      &btn_mode_roads_, &btn_mode_fences_, &btn_mode_placement_,
      &btn_mode_objects_, &btn_mode_biomes_, &btn_mode_tiles_,
      &toggle_terrain_, &toggle_water_, &toggle_roads_, &toggle_fences_,
      &toggle_placement_, &toggle_objects_, &toggle_biomes_, &toggle_tiles_,
      &btn_shader_wirebound_, &toggle_shaders_, &arrow_select_, &arrow_selected_,
      &save_data_, &load_data_);
}

// returns "stop render"
bool UiTabMenu::Render() {
  bool stop_show = Base::RenderBack(true);
//  if (!Base::BackIsReady()) {
//    return stop_show;
//  }

  ui_shared_resources_.static_sprite_shader_.Bind();

  btn_mode_terrain_.Render();
  btn_mode_water_.Render();
  btn_mode_roads_.Render();
  btn_mode_fences_.Render();
  btn_mode_placement_.Render();
  btn_mode_objects_.Render();
  btn_mode_biomes_.Render();
  btn_mode_tiles_.Render();

  toggle_terrain_.Render();
  toggle_water_.Render();
  toggle_roads_.Render();
  toggle_fences_.Render();
  toggle_placement_.Render();
  toggle_objects_.Render();
  toggle_biomes_.Render();
  toggle_tiles_.Render();

  btn_shader_wirebound_.Render();

  toggle_shaders_.Render();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  arrow_select_.SetRotate(arrow_select_angle_);
  arrow_selected_.SetRotate(arrow_selected_angle_);

  arrow_select_.Render();
  arrow_selected_.Render();
  save_data_.Render();
  load_data_.Render();

  return stop_show;
}

void UiTabMenu::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  btn_mode_terrain_.RenderPicking();
  btn_mode_water_.RenderPicking();
  btn_mode_roads_.RenderPicking();
  btn_mode_fences_.RenderPicking();
  btn_mode_placement_.RenderPicking();
  btn_mode_objects_.RenderPicking();
  btn_mode_biomes_.RenderPicking();
  btn_mode_tiles_.RenderPicking();

  toggle_terrain_.RenderPicking();
  toggle_water_.RenderPicking();
  toggle_roads_.RenderPicking();
  toggle_fences_.RenderPicking();
  toggle_placement_.RenderPicking();
  toggle_objects_.RenderPicking();
  toggle_biomes_.RenderPicking();
  toggle_tiles_.RenderPicking();

  btn_shader_wirebound_.RenderPicking();

  toggle_shaders_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  arrow_select_.RenderPicking();
  arrow_selected_.RenderPicking();
  save_data_.RenderPicking();
  load_data_.RenderPicking();
}

bool UiTabMenu::Press(int id) {
  bool handled = ui_event_handler_.Press(id);
  if (id != pin_.GetId()) {
    return handled;
  }
  if (!do_show_) {
    Hide();
  }
  return handled;
}

void UiTabMenu::Release() {
  ui_event_handler_.Release();
}

void UiTabMenu::SetSelectedArrow(float angle) {
  arrow_selected_angle_ = angle;
}

void UiTabMenu::SetSelectArrow(float angle) {
  arrow_select_angle_ = angle;
}

UiTipWindow::UiTipWindow(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    UiDynamicSprite&& text)
    : UiWindowAppear(std::move(sprite), size_scale,
                     std::move(pin), ui_shared_resources,
                     window_queue),
      text_(std::move(text)) { // 1
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &text_); // 2
  speed_ = 2.0f;
}

UiTipWindow::UiTipWindow(UiTipWindow&& other) noexcept
    : Base(std::move(other)),
      text_(std::move(other.text_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &text_);
}

bool UiTipWindow::Render() {
  UpdateHoverState(ui_shared_resources_.global_glfw_callback_data_.hovered_id);
  bool stop_show = Base::RenderBack(hovered_);
  if (!Base::BackIsReady()) {
    return stop_show;
  }
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  text_.Render();
  return stop_show;
}

void UiTipWindow::RenderPicking() {
  Base::RenderPickingBack();
}

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

UiWindowPopUp::UiWindowPopUp(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    LocalTransform start_transform,
    LocalTransform end_transform)
    : UiWindowBase(std::move(sprite), size_scale,
                   std::move(pin), ui_shared_resources,
                   window_queue),
      start_transform_(start_transform),
      end_transform_(end_transform) {}

UiWindowPopUp::UiWindowPopUp(UiWindowPopUp&& other) noexcept
    : UiWindowBase(std::move(other)),
      cur_transform_(other.cur_transform_),
      start_transform_(other.start_transform_),
      end_transform_(other.end_transform_) {}

bool UiWindowPopUp::RenderBack(bool show) {
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
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
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sprite_.RenderPicking();
  pin_.RenderPicking();
}

void UiWindowPopUp::CubicInterpolation() {
  cur_transform_.translate =
      start_transform_.translate
      + progress_ * (end_transform_.translate - start_transform_.translate);
  cur_transform_.rotate =
      start_transform_.rotate
      + progress_ * (end_transform_.rotate - start_transform_.rotate);
  cur_transform_.scale =
      start_transform_.scale
      + progress_ * (end_transform_.scale - start_transform_.scale);
}

UiSettings::UiSettings(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    LocalTransform start_transform,
    LocalTransform end_transform,
    UiDynamicSprite&& resolution_label,
    UiDynamicSprite&& resolution_left,
    UiDynamicSprite&& resolution_right,
    UiDynamicSprite&& resolution,
    UiToggle4&& toggle_fullscreen,
    UiSliderH2 sensitivity,
    UiSliderH2&& sound,
    UiToggle4&& toggle_sound,
    UiSliderH2&& music, UiToggle4&& toggle_music,
    UiDynamicSprite&& tip_info_label,
    UiDynamicSprite&& tip_info,
    UiToggle4&& toggle_tip_info)
    : UiWindowPopUp(std::move(sprite), size_scale,
                    std::move(pin), ui_shared_resources,
                    window_queue,
                    start_transform, end_transform),
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
      ui_event_handler_({
          &pin_, &resolution_left_, &resolution_right_,
          &toggle_fullscreen_, &sensitivity_,
          &sound_, &toggle_sound_, &music_, &toggle_music_,
          &tip_info_, &toggle_tip_info_}) {
  hierarchy_ = UiHierarchy(
      &sprite_, &pin_, &resolution_label_, &resolution_left_,
      &resolution_right_, &resolution_, &toggle_fullscreen_,
      &sensitivity_, &sound_, &toggle_sound_,
      &music_, &toggle_music_, &tip_info_label_, &tip_info_,
      &toggle_tip_info_);
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
      ui_event_handler_({
          &pin_, &resolution_left_, &resolution_right_,
          &toggle_fullscreen_, &sensitivity_,
          &sound_, &toggle_sound_, &music_, &toggle_music_,
          &tip_info_, &toggle_tip_info_}) {
  hierarchy_ = UiHierarchy(
      &sprite_, &pin_, &resolution_label_, &resolution_left_,
      &resolution_right_, &resolution_, &toggle_fullscreen_,
      &sensitivity_, &sound_, &toggle_sound_,
      &music_, &toggle_music_, &tip_info_label_, &tip_info_,
      &toggle_tip_info_);
}

// returns "stop render"
bool UiSettings::Render() {
  UpdateHoverState(ui_shared_resources_.global_glfw_callback_data_.hovered_id);
  ui_shared_resources_.tex_ui_.Bind();
  bool stop_show = Base::RenderBack(hovered_);
//  if (!Base::BackIsReady()) {
//    return stop_show;
//  }
  ui_shared_resources_.dynamic_sprite_shader_.Bind();

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

  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;

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

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(resolution_label_, "resolution", 0.05f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(tip_info_label_, "show tips", 0.05f, glm::vec2{0.0f});

  return stop_show;
}

void UiSettings::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

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

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(resolution_label_, "resolution", 0.05f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(tip_info_label_, "show tips", 0.05f, glm::vec2{0.0f});
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

void UiSettings::Release() {
  ui_event_handler_.Release();
}

bool UiSettings::Scroll(GLuint id, float yoffset) {
  return music_.Scroll(id, yoffset) ||
         sound_.Scroll(id, yoffset) ||
         sensitivity_.Scroll(id, yoffset);
}

UiWaterLayerConfig::UiWaterLayerConfig(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    LocalTransform start_transform,
    LocalTransform end_transform,
    UiDynamicSprite&& sprite_layer,
    UiDynamicSprite&& text_layer,
    UiToggle4&& toggle_layer,
    UiDynamicSprite&& scale_text,
    UiSliderH3&& scale,
    UiDynamicSprite&& fetch_text,
    UiSliderH3&& fetch,
    UiDynamicSprite&& spread_blend_text,
    UiSliderH3&& spread_blend,
    UiDynamicSprite&& swell_text,
    UiSliderH3&& swell,
    UiDynamicSprite&& peak_enhancement_text,
    UiSliderH3&& peak_enhancement,
    UiDynamicSprite&& short_waves_fade_text,
    UiSliderH3&& short_waves_fade,
    UiDynamicSprite&& lambda_text,
    UiSliderH3&& lambda)
    : UiWindowPopUp(std::move(sprite), size_scale,
                    std::move(pin), ui_shared_resources,
                    window_queue,
                    start_transform, end_transform),
      sprite_layer_(std::move(sprite_layer)),
      text_layer_(std::move(text_layer)),
      toggle_layer_(std::move(toggle_layer)),
      scale_(std::move(scale)),
      scale_text_(std::move(scale_text)),
      fetch_(std::move(fetch)),
      fetch_text_(std::move(fetch_text)),
      spread_blend_(std::move(spread_blend)),
      spread_blend_text_(std::move(spread_blend_text)),
      swell_(std::move(swell)),
      swell_text_(std::move(swell_text)),
      peak_enhancement_(std::move(peak_enhancement)),
      peak_enhancement_text_(std::move(peak_enhancement_text)),
      short_waves_fade_(std::move(short_waves_fade)),
      short_waves_fade_text_(std::move(short_waves_fade_text)),
      lambda_(std::move(lambda)),
      lambda_text_(std::move(lambda_text)),
      ui_event_handler_({
          &pin_, &toggle_layer_, &scale_, &fetch_, &spread_blend_, &swell_,
          &peak_enhancement_, &short_waves_fade_, &lambda_
      }) {
  hierarchy_ = UiHierarchy(
      &sprite_, &pin_, &sprite_layer_, &text_layer_,
      &toggle_layer_, &scale_, &scale_text_,
      &fetch_, &fetch_text_, &spread_blend_, &spread_blend_text_,
      &swell_, &swell_text_, &peak_enhancement_, &peak_enhancement_text_,
      &short_waves_fade_, &short_waves_fade_text_, &lambda_, &lambda_text_);
}

UiWaterLayerConfig::UiWaterLayerConfig(UiWaterLayerConfig&& other) noexcept
    : Base(std::move(other)),
      sprite_layer_(std::move(other.sprite_layer_)),
      text_layer_(std::move(other.text_layer_)),
      toggle_layer_(std::move(other.toggle_layer_)),
      scale_(std::move(other.scale_)),
      scale_text_(std::move(other.scale_text_)),
      fetch_(std::move(other.fetch_)),
      fetch_text_(std::move(other.fetch_text_)),
      spread_blend_(std::move(other.spread_blend_)),
      spread_blend_text_(std::move(other.spread_blend_text_)),
      swell_(std::move(other.swell_)),
      swell_text_(std::move(other.swell_text_)),
      peak_enhancement_(std::move(other.peak_enhancement_)),
      peak_enhancement_text_(std::move(other.peak_enhancement_text_)),
      short_waves_fade_(std::move(other.short_waves_fade_)),
      short_waves_fade_text_(std::move(other.short_waves_fade_text_)),
      lambda_(std::move(other.lambda_)),
      lambda_text_(std::move(other.lambda_text_)),
      modified_(other.modified_),
      ui_event_handler_({
          &pin_, &toggle_layer_, &scale_, &fetch_, &spread_blend_, &swell_,
          &peak_enhancement_, &short_waves_fade_, &lambda_
      }) {
  hierarchy_ = UiHierarchy(
      &sprite_, &pin_, &sprite_layer_, &text_layer_,
      &toggle_layer_, &scale_, &scale_text_,
      &fetch_, &fetch_text_, &spread_blend_, &spread_blend_text_,
      &swell_, &swell_text_, &peak_enhancement_, &peak_enhancement_text_,
      &short_waves_fade_, &short_waves_fade_text_, &lambda_, &lambda_text_);
}

// returns "stop render"
bool UiWaterLayerConfig::Render() {
  UpdateHoverState(ui_shared_resources_.global_glfw_callback_data_.hovered_id);
  bool stop_show = Base::RenderBack(hovered_);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  sprite_layer_.SetParentTransform(Base::cur_transform_);
  text_layer_.SetParentTransform(Base::cur_transform_);
  toggle_layer_.SetParentTransform(Base::cur_transform_);
  scale_.SetParentTransform(Base::cur_transform_);
  scale_text_.SetParentTransform(Base::cur_transform_);
  fetch_.SetParentTransform(Base::cur_transform_);
  fetch_text_.SetParentTransform(Base::cur_transform_);
  spread_blend_.SetParentTransform(Base::cur_transform_);
  spread_blend_text_.SetParentTransform(Base::cur_transform_);
  swell_.SetParentTransform(Base::cur_transform_);
  swell_text_.SetParentTransform(Base::cur_transform_);
  peak_enhancement_.SetParentTransform(Base::cur_transform_);
  peak_enhancement_text_.SetParentTransform(Base::cur_transform_);
  short_waves_fade_.SetParentTransform(Base::cur_transform_);
  short_waves_fade_text_.SetParentTransform(Base::cur_transform_);
  lambda_.SetParentTransform(Base::cur_transform_);
  lambda_text_.SetParentTransform(Base::cur_transform_);

  /// shader & transform matrix already bind by Base class
  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  sprite_layer_.Render();
  toggle_layer_.Render();
  scale_.Render(mouse_pos);
  fetch_.Render(mouse_pos);
  spread_blend_.Render(mouse_pos);
  swell_.Render(mouse_pos);
  peak_enhancement_.Render(mouse_pos);
  short_waves_fade_.Render(mouse_pos);
  lambda_.Render(mouse_pos);

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(text_layer_, "layer name", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(scale_text_, "scale", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(fetch_text_, "fetch", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(spread_blend_text_, "spread blend", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(swell_text_, "swell", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(peak_enhancement_text_, "peak enhancement", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(short_waves_fade_text_, "short waves fade", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(lambda_text_, "lambda", 0.1f, glm::vec2{0.0f});

  return stop_show;
}

void UiWaterLayerConfig::RenderPicking() {
  Base::RenderPickingBack();
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sprite_layer_.RenderPicking();
  toggle_layer_.RenderPicking();
  scale_.RenderPicking();
  fetch_.RenderPicking();
  spread_blend_.RenderPicking();
  swell_.RenderPicking();
  peak_enhancement_.RenderPicking();
  short_waves_fade_.RenderPicking();
  lambda_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(text_layer_, "layer name", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(scale_text_, "scale", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(fetch_text_, "fetch", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(spread_blend_text_, "spread blend", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(swell_text_, "swell", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(peak_enhancement_text_, "peak enhancement", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(short_waves_fade_text_, "short waves fade", 0.1f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(lambda_text_, "lambda", 0.1f, glm::vec2{0.0f});
}

bool UiWaterLayerConfig::Press(int id) {
  modified_ = ui_event_handler_.Press(id);
  return modified_;
}

void UiWaterLayerConfig::Release() {
  ui_event_handler_.Release();
}

void UiWaterLayerConfig::UpdateHoverState(int id) {
  hovered_ = (id >= sprite_.GetId() && id <= lambda_text_.GetId());
}

bool UiWaterLayerConfig::Scroll(GLuint id, float yoffset) {
  return scale_.Scroll(id, yoffset) ||
         fetch_.Scroll(id, yoffset) ||
         spread_blend_.Scroll(id, yoffset) ||
         swell_.Scroll(id, yoffset) ||
         peak_enhancement_.Scroll(id, yoffset) ||
         short_waves_fade_.Scroll(id, yoffset) ||
         lambda_.Scroll(id, yoffset);
}

bool UiWaterLayerConfig::Modified() {
  if (modified_) {
    modified_ = false;
    return true;
  }
  return false;
}

OceanLayerTraits UiWaterLayerConfig::GetOceanLayerTraits() const {
  return {
      scale_.GetProgress(),
      fetch_.GetProgress(),
      spread_blend_.GetProgress(),
      swell_.GetProgress(),
      peak_enhancement_.GetProgress(),
      short_waves_fade_.GetProgress(),
      lambda_.GetProgress()
  };
}

//TODO: deprecated
UiEditFences::UiEditFences(
    UiSharedResources& ui_shared_resources,
    UiDynamicSprite&& desk,
    UiDynamicSprite&& accept,
    UiDynamicSprite&& name,
    UiDynamicSprite&& name_back,
    UiSlider2D&& color_palette,
    UiSliderH2&& color_brightness,
    UiDynamicSprite&& color_indicator,
    UiDynamicSprite&& type_back,
    UiDynamicSprite&& type_text,
    UiDynamicSprite&& type_prev,
    UiDynamicSprite&& type_next)
    : UiBase(desk.GetId(), {}),
      desk_(std::move(desk)),
      accept_(std::move(accept)),
      name_(std::move(name)),
      name_back_(std::move(name_back)),
      color_palette_(std::move(color_palette)),
      color_brightness_(std::move(color_brightness)),
      color_indicator_(std::move(color_indicator)),
      type_back_(std::move(type_back)),
      type_text_(std::move(type_text)),
      type_prev_(std::move(type_prev)),
      type_next_(std::move(type_next)),
      ui_event_handler_({
          &accept_, &name_back_, &color_palette_,
          &color_brightness_, &type_prev_, &type_next_
      }),
      ui_shared_resources_(ui_shared_resources) {
  gUiComponents[accept_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[name_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[name_back_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[color_palette_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[color_brightness_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[color_indicator_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[type_back_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[type_text_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[type_prev_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[type_next_.GetId() - details::kIdOffsetUi].parent_id_
      = desk.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiEditFences::UiEditFences(UiEditFences&& other) noexcept
    : UiBase(std::move(other)),
      desk_(std::move(other.desk_)),
      accept_(std::move(other.accept_)),
      name_(std::move(other.name_)),
      name_back_(std::move(other.name_back_)),
      color_palette_(std::move(other.color_palette_)),
      color_brightness_(std::move(other.color_brightness_)),
      color_indicator_(std::move(other.color_indicator_)),
      type_back_(std::move(other.type_back_)),
      type_text_(std::move(other.type_text_)),
      type_prev_(std::move(other.type_prev_)),
      type_next_(std::move(other.type_next_)),
      ui_event_handler_({
          &accept_, &name_back_, &color_palette_,
          &color_brightness_, &type_prev_, &type_next_
      }),
      ui_shared_resources_(other.ui_shared_resources_) {
  gUiComponents[desk_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

bool UiEditFences::Press(int id) {
  return ui_event_handler_.Press(id);
}

void UiEditFences::Release() {
  ui_event_handler_.Release();
}

bool UiEditFences::Scroll(GLuint id, float yoffset) {
  return color_palette_.Scroll(id, yoffset) ||
         color_brightness_.Scroll(id, yoffset);
}

void UiEditFences::Render() {
  ui_shared_resources_.static_sprite_shader_.Bind();

  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;

  desk_.Render();
  accept_.Render();
  name_back_.Render();
  color_palette_.Render(mouse_pos);
  color_brightness_.Render(mouse_pos);
  //TODO: set indicator color
  color_indicator_.Render();
  type_back_.Render();
  type_prev_.Render();
  type_next_.Render();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(name_, "name", 1.0f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(type_text_, "type", 1.0f, glm::vec2{0.0f});
}

void UiEditFences::RenderPicking() {
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  desk_.RenderPicking();
  accept_.RenderPicking();
  name_back_.RenderPicking();
  color_palette_.RenderPicking();
  color_brightness_.RenderPicking();
  //TODO: set indicator color
  color_indicator_.RenderPicking();
  type_back_.RenderPicking();
  type_prev_.RenderPicking();
  type_next_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(name_, "name", 1.0f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(type_text_, "type", 1.0f, glm::vec2{0.0f});

}
