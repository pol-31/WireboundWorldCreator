#include "UiComplex.h"

#include "../common/UiDebugger.h"
#include "../common/ShadersBinding.h"
#include "../common/TextRenderer.h"
#include "../common/Details.h"

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
    : UiBase(sprite0.GetId(), {}),
      sprites_{
          {std::move(sprite0), std::move(sprite10), std::move(sprite20),
           std::move(sprite30), std::move(sprite40), std::move(sprite50),
           std::move(sprite60), std::move(sprite70), std::move(sprite80),
           std::move(sprite90), std::move(sprite100)}} {
  for (const auto& sprite : sprites_) {
    gUiComponents[sprite.GetId() - details::kIdOffsetUi].parent_id_
        = sprites_[0].GetId();
  }
  // reset father to default
  gUiComponents[sprites_[0].GetId() - details::kIdOffsetUi].parent_id_
      = 0;
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

UiLoading::UiLoading(UiLoading&& other) noexcept
    : UiBase(std::move(other)),
      sprites_(std::move(other.sprites_)) {
  gUiComponents[sprites_[0].GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiLoading::Render(float progress) {
  int idx = static_cast<int>(std::round(progress * 10.0f));
  idx = std::clamp(idx, 0, 10);
  sprites_[idx].Render();
}

void UiLoading::RenderPicking() const {
  sprites_[0].RenderPicking();
}

void UiLoading::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  for (auto& sprite : sprites_) {
    sprite.UpdateTransform();
  }
}

void UiLoading::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

const float UiSlider2D::kTrackWidthFactor = 0.9f;
const float UiSlider2D::kTrackHeightFactor = 0.8f;

UiSlider2D::UiSlider2D(
    UiDynamicSprite&& palette,
    UiDynamicSprite&& cursor,
    glm::vec2 scale)
    : UiBase(palette.GetId(), {}),
      palette_(std::move(palette)),
      cursor_(std::move(cursor)),
      length_({
          kTrackWidthFactor *
              (palette.GetRightBorder() - palette.GetLeftBorder()),
          kTrackHeightFactor *
              (palette.GetTopBorder() - palette.GetBottomBorder())}),
      centre_({
          (palette.GetRightBorder() + palette.GetLeftBorder()) / 2.0f,
          (palette.GetTopBorder() + palette.GetBottomBorder()) / 2.0f}),
      scale_(scale) {
  gUiComponents[cursor_.GetId() - details::kIdOffsetUi].parent_id_
      = palette_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiSlider2D::UiSlider2D(UiSlider2D&& other) noexcept
    : UiBase(std::move(other)),
      palette_(std::move(other.palette_)),
      cursor_(std::move(other.cursor_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  gUiComponents[palette_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiSlider2D::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    SetMousePos(mouse_pos);
  }
  palette_.Render();
  cursor_.Render();
}

void UiSlider2D::RenderIcon() {
  cursor_.Render();
}

void UiSlider2D::RenderPicking() const {
  palette_.RenderPicking();
  if (debug::gUiAltMode) {
    cursor_.RenderPicking();
  }
}

void UiSlider2D::SetMousePos(glm::vec2 mouse_pos) {
  glm::vec2 half_length_ = length_ / 2.0f;
  glm::vec2 offset = glm::clamp(
      mouse_pos - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  cursor_.SetTranslate(offset);
}

void UiSlider2D::SetProgress(glm::vec2 progress) {
  progress_ = progress;
  glm::vec2 half_length_ = length_ / 2.0f;
  glm::vec2 offset = progress_ * length_ - half_length_;
  cursor_.SetTranslate(offset);
}

data::TextId UiSlider2D::Hover(std::uint32_t id) {
  return palette_.Hover();
}

bool UiSlider2D::Scroll(GLuint id, float yoffset) {
  if (id != palette_.GetId() || id != cursor_.GetId()) {
    return false;
  }
  glm::vec2 dir = centre_; // unf we can't, we don't have quick mouse pos
  float factor = 0.01f * yoffset;
  glm::vec2 progress = progress_;
  if (glfwGetKey(gWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    progress.x = std::clamp(progress_.x + factor, 0.0f, 1.0f);
  } else {
    progress.y = std::clamp(progress_.y + factor, 0.0f, 1.0f);
  }
  SetProgress(progress);
  return true;
}

glm::vec2 UiSlider2D::GetProgress() const {
  return (glm::vec2{1.0f} - progress_) * scale_;
}

float UiSlider2D::GetProgressX() const {
  return (1.0f - progress_.x) * scale_.x;
}

float UiSlider2D::GetProgressY() const {
  return (1.0f - progress_.y) * scale_.y;
}

void UiSlider2D::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  palette_.UpdateTransform();
  cursor_.UpdateTransform();
  length_ =
      {kTrackWidthFactor *
           (palette_.GetRightBorder() - palette_.GetLeftBorder()),
       kTrackHeightFactor *
           (palette_.GetTopBorder() - palette_.GetBottomBorder())};
  centre_ =
      {(palette_.GetRightBorder() + palette_.GetLeftBorder()) / 2.0f,
       (palette_.GetTopBorder() + palette_.GetBottomBorder()) / 2.0f};
  glm::vec2 related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  SetMousePos(related_pos);
}

void UiSlider2D::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(transform.translate.x, transform.translate.y,
                  transform.scale);
}

void UiSlider2D::SetParentTransform(LocalTransform transform) {
  palette_.SetParentTransform(transform);
  cursor_.SetParentTransform(transform);
}

UiTopWindowBase::UiTopWindowBase(
    UiDynamicSprite&& desk,
    UiDynamicSprite&& shadow,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue)
    : UiBase(desk.GetId(), {}),
      desk_(std::move(desk)),
      shadow_(std::move(shadow)),
      size_scale_(size_scale),
      ui_shared_resources_(ui_shared_resources),
      window_queue_(window_queue) {
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  desk_.SetScale(size_scale);
}

UiTopWindowBase::UiTopWindowBase(UiTopWindowBase&& other) noexcept
    : UiBase(std::move(other)),
      desk_(std::move(other.desk_)),
      shadow_(std::move(other.shadow_)),
      size_scale_(other.size_scale_),
      ui_shared_resources_(other.ui_shared_resources_),
      window_queue_(other.window_queue_) {
  gUiComponents[desk_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiTopWindowBase::Show() {
  window_queue_.SetTopWindow(this);
}

void UiTopWindowBase::Hide() {
  window_queue_.SetTopWindow(nullptr);
}

void UiTopWindowBase::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

UiCaution::UiCaution(
    UiDynamicSprite&& desk,
    UiDynamicSprite&& shadow,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    UiDynamicSprite&& text)
    : UiTopWindowBase(std::move(desk), std::move(shadow), size_scale,
                      ui_shared_resources, window_queue),
      text_(std::move(text)) {
  gUiComponents[text_.GetId() - details::kIdOffsetUi].parent_id_
      = desk_.GetId();

  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UiTopWindowBase::UpdateTransform();
}

UiCaution::UiCaution(UiCaution&& other) noexcept
    : Base(std::move(other)),
      text_(std::move(other.text_)) {
  gUiComponents[desk_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

bool UiCaution::Render() {
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  desk_.Render();
  shadow_.Render();
  //  text_.Render();
  return false;
}

void UiCaution::RenderPicking() {
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  desk_.RenderPicking();
  shadow_.RenderPicking();
  //  text_.RenderPicking();
}

void UiCaution::Press(int id) {
  if (id == shadow_.GetId()) {
    Hide();
  }
}

void UiCaution::Release() {}

data::TextId UiCaution::Hover(int id) {
  return data::TextId::kNone;
}

void UiCaution::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  desk_.UpdateTransform();
  shadow_.UpdateTransform();
  text_.UpdateTransform();
}

UiConfirmation::UiConfirmation(
    UiDynamicSprite&& desk,
    UiDynamicSprite&& shadow,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    UiDynamicSprite&& btn_accept,
    UiDynamicSprite&& btn_decline,
    UiDynamicSprite&& text)
    : UiTopWindowBase(std::move(desk), std::move(shadow), size_scale,
                      ui_shared_resources, window_queue),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      text_(std::move(text)),
      ui_event_handler_({&btn_accept_, &btn_decline_}) {
  gUiComponents[btn_accept_.GetId() - details::kIdOffsetUi].parent_id_
      = desk_.GetId();
  gUiComponents[btn_decline_.GetId() - details::kIdOffsetUi].parent_id_
      = desk_.GetId();
  gUiComponents[text_.GetId() - details::kIdOffsetUi].parent_id_
      = desk_.GetId();

  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UiTopWindowBase::UpdateTransform();
}

UiConfirmation::UiConfirmation(UiConfirmation&& other) noexcept
    : Base(std::move(other)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      text_(std::move(other.text_)),
      ui_event_handler_({&btn_accept_, &btn_decline_}) {
  gUiComponents[desk_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

bool UiConfirmation::Render() {
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  desk_.Render();
  shadow_.Render();
  btn_accept_.Render();
  btn_decline_.Render();
  //  text_.Render();
  return false;
}

void UiConfirmation::RenderPicking() {
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  desk_.RenderPicking();
  shadow_.RenderPicking();
  btn_accept_.RenderPicking();
  btn_decline_.RenderPicking();
  //  text_.RenderPicking();
}

void UiConfirmation::Press(int id) {
//  ui_event_handler_.Press(id);
  if (id == btn_accept_.GetId()) {
    callable_();
  } else if (id != shadow_.GetId() && id != desk_.GetId()) {
    Hide();
  }
}

void UiConfirmation::Release() {
  ui_event_handler_.Release();
}

data::TextId UiConfirmation::Hover(int id) {
  return ui_event_handler_.Hover(id);
}

void UiConfirmation::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  desk_.UpdateTransform();
  shadow_.UpdateTransform();

  /// children
  btn_accept_.UpdateTransform();
  btn_decline_.UpdateTransform();
  text_.UpdateTransform();
}

void UiConfirmation::SetText(std::string_view text) {
  std::cout << "text set? not implemented" << std::endl;
}

void UiConfirmation::SetCallable(std::function<void()>&& callable) {
  callable_ = std::move(callable);
}

UiFile::UiFile(
    UiDynamicSprite&& desk,
    UiDynamicSprite&& shadow,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    UiDynamicSprite&& btn_accept,
    UiDynamicSprite&& btn_decline,
    UiTextLabel&& label,
    UiTextInput&& text)
    : UiTopWindowBase(std::move(desk), std::move(shadow), size_scale,
                      ui_shared_resources, window_queue),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      label_(std::move(label)),
      text_(std::move(text)),
      ui_event_handler_({&btn_accept_/*, &btn_decline_*/}) {
  gUiComponents[btn_accept_.GetId() - details::kIdOffsetUi].parent_id_
      = desk_.GetId();
  gUiComponents[btn_decline_.GetId() - details::kIdOffsetUi].parent_id_
      = desk_.GetId();
  gUiComponents[label_.GetId() - details::kIdOffsetUi].parent_id_
      = desk_.GetId();
  gUiComponents[text_.GetId() - details::kIdOffsetUi].parent_id_
      = desk_.GetId();

  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UiTopWindowBase::UpdateTransform();
  label_.SetText("label");
}

UiFile::UiFile(UiFile&& other) noexcept
    : Base(std::move(other)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      label_(std::move(other.label_)),
      text_(std::move(other.text_)),
      ui_event_handler_({&btn_accept_/*, &btn_decline_*/}) {
  gUiComponents[desk_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

bool UiFile::Render() {
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  desk_.Render();
  shadow_.Render();
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
  shadow_.RenderPicking();
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

data::TextId UiFile::Hover(int id) {
  return ui_event_handler_.Hover(id);
}

void UiFile::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  desk_.UpdateTransform();
  shadow_.UpdateTransform();

  /// children
  btn_accept_.UpdateTransform();
  btn_decline_.UpdateTransform();
  label_.UpdateTransform();
  text_.UpdateTransform();
}

UiWindowBase::UiWindowBase(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue)
    : UiBase(sprite.GetId(), {}),
      sprite_(std::move(sprite)),
      size_scale_(size_scale),
      pin_(std::move(pin)),
      ui_shared_resources_(ui_shared_resources),
      window_queue_(window_queue),
      speed_(4.0f) {
  gUiComponents[pin_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  sprite_.SetScale(size_scale);
  //todo; should be called after Derived ctor
//  UpdateTransform();
}

UiWindowBase::UiWindowBase(UiWindowBase&& other) noexcept
    : UiBase(std::move(other)),
      sprite_(std::move(other.sprite_)),
      size_scale_(other.size_scale_),
      pin_(std::move(other.pin_)),
      ui_shared_resources_(other.ui_shared_resources_),
      window_queue_(other.window_queue_),
      speed_(other.speed_),
      progress_(other.progress_),
      back_ready_(other.back_ready_) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

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

void UiWindowBase::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
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

    UiToggle&& toggle_terrain,
    UiToggle&& toggle_water,
    UiToggle&& toggle_roads,
    UiToggle&& toggle_fences,
    UiToggle&& toggle_placement,
    UiToggle&& toggle_objects,
    UiToggle&& toggle_biomes,
    UiToggle&& toggle_tiles,

    UiStaticSprite&& btn_shader_wirebound,
    UiToggle&& toggle_shaders,

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
  gUiComponents[btn_mode_terrain_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_water_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_roads_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_fences_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_placement_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_objects_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_biomes_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_tiles_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[toggle_terrain_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_water_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_roads_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_fences_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_placement_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_objects_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_biomes_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_tiles_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[btn_shader_wirebound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_shaders_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[arrow_select_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[arrow_selected_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[save_data_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[load_data_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UiWindowBase::UpdateTransform();
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
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
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

data::TextId UiTabMenu::Hover(int id) {
  return ui_event_handler_.Hover(id);
}

void UiTabMenu::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  sprite_.UpdateTransform();
  pin_.UpdateTransform();

  /// children
  btn_mode_terrain_.UpdateTransform();
  btn_mode_water_.UpdateTransform();
  btn_mode_roads_.UpdateTransform();
  btn_mode_fences_.UpdateTransform();
  btn_mode_placement_.UpdateTransform();
  btn_mode_objects_.UpdateTransform();
  btn_mode_biomes_.UpdateTransform();
  btn_mode_tiles_.UpdateTransform();

  toggle_terrain_.UpdateTransform();
  toggle_water_.UpdateTransform();
  toggle_roads_.UpdateTransform();
  toggle_fences_.UpdateTransform();
  toggle_placement_.UpdateTransform();
  toggle_objects_.UpdateTransform();
  toggle_biomes_.UpdateTransform();
  toggle_tiles_.UpdateTransform();

  btn_shader_wirebound_.UpdateTransform();
  toggle_shaders_.UpdateTransform();

  arrow_select_.UpdateTransform();
  arrow_selected_.UpdateTransform();
  save_data_.UpdateTransform();
  load_data_.UpdateTransform();
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
      text_(std::move(text)) {
  gUiComponents[text_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UiWindowBase::UpdateTransform();
  speed_ = 2.0f;
}

UiTipWindow::UiTipWindow(UiTipWindow&& other) noexcept
    : Base(std::move(other)),
      text_(std::move(other.text_)) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

bool UiTipWindow::Render() {
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

data::TextId UiTipWindow::Hover(int id) {
  if (id >= sprite_.GetId() && id <= text_.GetId()) {
    hovered_ = true;
  } else {
    hovered_ = false;
  }
  return data::TextId::kNone;
}

void UiTipWindow::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  sprite_.UpdateTransform();
  pin_.UpdateTransform();

  /// children
  text_.UpdateTransform();
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
    UiToggle&& toggle_fullscreen,
    UiSliderH2 sensitivity,
    UiDynamicSprite keyboard,
    UiSliderH2&& sound,
    UiToggle&& toggle_sound,
    UiSliderH2&& music,
    UiToggle&& toggle_music,
    UiDynamicSprite&& tip_info_label,
    UiDynamicSprite&& tip_info,
    UiToggle&& toggle_tip_info)
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
      keyboard_(std::move(keyboard)),
      sound_(std::move(sound)),
      toggle_sound_(std::move(toggle_sound)),
      music_(std::move(music)),
      toggle_music_(std::move(toggle_music)),
      tip_info_label_(std::move(tip_info_label)),
      tip_info_(std::move(tip_info)),
      toggle_tip_info_(std::move(toggle_tip_info)),
      ui_event_handler_({
          &pin_, &resolution_left_, &resolution_right_,
          &toggle_fullscreen_, &sensitivity_, &keyboard_,
          &sound_, &toggle_sound_, &music_, &toggle_music_,
          &tip_info_, &toggle_tip_info_}) {
  gUiComponents[resolution_label_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[resolution_left_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[resolution_right_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[resolution_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_fullscreen_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sensitivity_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[keyboard_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_sound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[music_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_music_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[tip_info_label_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[tip_info_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_tip_info_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UiWindowBase::UpdateTransform();
}

UiSettings::UiSettings(UiSettings&& other) noexcept
    : Base(std::move(other)),
      resolution_label_(std::move(other.resolution_label_)),
      resolution_left_(std::move(other.resolution_left_)),
      resolution_right_(std::move(other.resolution_right_)),
      resolution_(std::move(other.resolution_)),
      toggle_fullscreen_(std::move(other.toggle_fullscreen_)),
      sensitivity_(std::move(other.sensitivity_)),
      keyboard_(std::move(other.keyboard_)),
      sound_(std::move(other.sound_)),
      toggle_sound_(std::move(other.toggle_sound_)),
      music_(std::move(other.music_)),
      toggle_music_(std::move(other.toggle_music_)),
      tip_info_label_(std::move(other.tip_info_label_)),
      tip_info_(std::move(other.tip_info_)),
      toggle_tip_info_(std::move(other.toggle_tip_info_)),
      ui_event_handler_({
          &pin_, &resolution_left_, &resolution_right_,
          &toggle_fullscreen_, &sensitivity_, &keyboard_,
          &sound_, &toggle_sound_, &music_, &toggle_music_,
          &tip_info_, &toggle_tip_info_}) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

// returns "stop render"
bool UiSettings::Render() {
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
  keyboard_.SetParentTransform(Base::cur_transform_);
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
  keyboard_.Render();
  sound_.Render(mouse_pos);
  toggle_sound_.Render();
  music_.Render(mouse_pos);
  toggle_music_.Render();
  tip_info_.Render();
  toggle_tip_info_.Render();

  music_.RenderIcon();
  sound_.RenderIcon();
  sensitivity_.RenderIcon();

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
  keyboard_.RenderPicking();
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

data::TextId UiSettings::Hover(int id) {
  data::TextId hovered_id = ui_event_handler_.Hover(id);
  if (hovered_id != data::TextId::kNone || id == Base::GetId()) {
    hovered_ = true;
  } else {
    hovered_ = false;
  }
  return hovered_id;
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

void UiSettings::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  resolution_label_.SetParentTransform(Base::cur_transform_);
  resolution_left_.SetParentTransform(Base::cur_transform_);
  resolution_right_.SetParentTransform(Base::cur_transform_);
  resolution_.SetParentTransform(Base::cur_transform_);
  toggle_fullscreen_.SetParentTransform(Base::cur_transform_);
  sensitivity_.SetParentTransform(Base::cur_transform_);
  keyboard_.SetParentTransform(Base::cur_transform_);
  sound_.SetParentTransform(Base::cur_transform_);
  toggle_sound_.SetParentTransform(Base::cur_transform_);
  music_.SetParentTransform(Base::cur_transform_);
  toggle_music_.SetParentTransform(Base::cur_transform_);
  tip_info_label_.SetParentTransform(Base::cur_transform_);
  tip_info_.SetParentTransform(Base::cur_transform_);
  toggle_tip_info_.SetParentTransform(Base::cur_transform_);
  /// back
  sprite_.UpdateTransform();
  pin_.UpdateTransform();

  /// children
  resolution_label_.UpdateTransform();
  resolution_left_.UpdateTransform();
  resolution_right_.UpdateTransform();
  resolution_.UpdateTransform();
  toggle_fullscreen_.UpdateTransform();
  sensitivity_.UpdateTransform();
  keyboard_.UpdateTransform();
  sound_.UpdateTransform();
  toggle_sound_.UpdateTransform();
  music_.UpdateTransform();
  toggle_music_.UpdateTransform();
  tip_info_label_.UpdateTransform();
  tip_info_.UpdateTransform();
  toggle_tip_info_.UpdateTransform();
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
    UiToggle&& toggle_layer,
    UiDynamicSprite&& scale_text,
    UiSliderH&& scale,
    UiDynamicSprite&& fetch_text,
    UiSliderH&& fetch,
    UiDynamicSprite&& spread_blend_text,
    UiSliderH&& spread_blend,
    UiDynamicSprite&& swell_text,
    UiSliderH&& swell,
    UiDynamicSprite&& peak_enhancement_text,
    UiSliderH&& peak_enhancement,
    UiDynamicSprite&& short_waves_fade_text,
    UiSliderH&& short_waves_fade,
    UiDynamicSprite&& lambda_text,
    UiSliderH&& lambda)
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
  gUiComponents[sprite_layer_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[text_layer_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_layer_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[scale_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[scale_text_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[fetch_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[fetch_text_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[spread_blend_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[spread_blend_text_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[swell_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[swell_text_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[peak_enhancement_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[peak_enhancement_text_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[short_waves_fade_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[short_waves_fade_text_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[lambda_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[lambda_text_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UiWindowBase::UpdateTransform();
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
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

// returns "stop render"
bool UiWaterLayerConfig::Render() {
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

  scale_.RenderIcon();
  fetch_.RenderIcon();
  spread_blend_.RenderIcon();
  swell_.RenderIcon();
  peak_enhancement_.RenderIcon();
  short_waves_fade_.RenderIcon();
  lambda_.RenderIcon();

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

data::TextId UiWaterLayerConfig::Hover(int id) {
  data::TextId hovered_id = ui_event_handler_.Hover(id);
  if (hovered_id != data::TextId::kNone || id == Base::GetId()) {
    hovered_ = true;
  } else {
    hovered_ = false;
  }
  return hovered_id;
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

void UiWaterLayerConfig::UpdateTransform(
    float x_translate, float y_translate, float scale) {
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

  /// back
  sprite_.UpdateTransform();
  pin_.UpdateTransform();

  /// children
  sprite_layer_.UpdateTransform();
  text_layer_.UpdateTransform();
  toggle_layer_.UpdateTransform();
  scale_.UpdateTransform();
  scale_text_.UpdateTransform();
  fetch_.UpdateTransform();
  fetch_text_.UpdateTransform();
  spread_blend_.UpdateTransform();
  spread_blend_text_.UpdateTransform();
  swell_.UpdateTransform();
  swell_text_.UpdateTransform();
  peak_enhancement_.UpdateTransform();
  peak_enhancement_text_.UpdateTransform();
  short_waves_fade_.UpdateTransform();
  short_waves_fade_text_.UpdateTransform();
  lambda_.UpdateTransform();
  lambda_text_.UpdateTransform();
}

//IUiEdit::IUiEdit()
//    : UiBase(data::VboIdMain::kSpareText2, {}, {}) {}


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

data::TextId UiEditFences::Hover(int id) {
  return ui_event_handler_.Hover(id);
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

  color_palette_.RenderIcon();
  color_brightness_.RenderIcon();

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

void UiEditFences::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  desk_.UpdateTransform();
  accept_.UpdateTransform();
  name_.UpdateTransform();
  name_back_.UpdateTransform();
  color_palette_.UpdateTransform();
  color_brightness_.UpdateTransform();
  color_indicator_.UpdateTransform();
  type_back_.UpdateTransform();
  type_text_.UpdateTransform();
  type_prev_.UpdateTransform();
  type_next_.UpdateTransform();
}

void UiEditFences::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}
