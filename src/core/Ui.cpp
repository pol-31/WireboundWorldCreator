#include "Ui.h"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "../modes/TerrainMode.h"
#include "../modes/WaterMode.h"
#include "../common/Colors.h"
#include "../common/ShadersBinding.h"

#include "../common/UiDebugger.h"

std::array<vbos::UiData, vbos::gVboIdSize> gUiComponents{};

LocalTransformLinear GetParentDbgTransform(size_t id) {
  auto parent_id = gUiComponents[id - details::kIdOffsetUi].parent_id_;
  LocalTransformLinear transform{};
  if (parent_id != 0) {
    transform = debug::gUiTransforms[
        4 * (parent_id - details::kIdOffsetUi)];
    transform.Apply(GetParentDbgTransform(parent_id));
  }
  return transform;
}

UiDynamicSprite::UiDynamicSprite(
    vbos::VboIdMain vbo_texture, vbos::VboIdText vbo_text,
    CallableType action)
    : UiCallable(vbo_texture, vbo_text, std::move(action)) {
  UpdateTransform();
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
}

UiDynamicSprite::UiDynamicSprite(UiDynamicSprite&& other) noexcept
    : UiCallable(std::move(other)),
      local_transform_(other.local_transform_),
      parent_transform_(other.parent_transform_),
      final_dbg_transform_(other.final_dbg_transform_) {
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
}

void UiDynamicSprite::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  final_dbg_transform_ = GetParentDbgTransform(ui_data_.id);
  auto dbg_transform = debug::gUiTransforms[4 * (ui_data_.id - details::kIdOffsetUi)];
  final_dbg_transform_.Apply(dbg_transform);
}

void UiDynamicSprite::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (ui_data_.id - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

float UiDynamicSprite::GetLeftBorder() const {
  auto final_scale = final_dbg_transform_.scale * local_transform_.scale
                     * parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto final_translate = final_dbg_transform_.translate + local_transform_.translate
                         + parent_transform_.translate;
  float width = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4]
                - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
  return final_translate.x - final_scale.x * width / 2.0f;
}

float UiDynamicSprite::GetRightBorder() const {
  auto final_scale = final_dbg_transform_.scale * local_transform_.scale
                     * parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto final_translate = final_dbg_transform_.translate + local_transform_.translate
                         + parent_transform_.translate;
  float width = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4]
                - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
  return final_translate.x + final_scale.x * width / 2.0f;
}

float UiDynamicSprite::GetTopBorder() const {
  auto final_scale = final_dbg_transform_.scale * local_transform_.scale
                     * parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto final_translate = final_dbg_transform_.translate + local_transform_.translate
                         + parent_transform_.translate;
  float height = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
  return final_translate.y + final_scale.y * height / 2.0f;
}

float UiDynamicSprite::GetBottomBorder() const {
  auto final_scale = final_dbg_transform_.scale * local_transform_.scale
                     * parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto final_translate = final_dbg_transform_.translate + local_transform_.translate
                         + parent_transform_.translate;
  float height = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                 - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
  return final_translate.y - final_scale.y * height / 2.0f;
}

void UiDynamicSprite::Render() {
  glm::mat3 transform{1.0f};
  transform = glm::translate(
      transform, final_dbg_transform_.translate + local_transform_.translate
                     + parent_transform_.translate);
  transform = glm::rotate(
      transform, local_transform_.rotate + parent_transform_.rotate);
  transform = glm::scale(
      transform, final_dbg_transform_.scale * local_transform_.scale
                     * parent_transform_.scale * glm::vec2{gResFactor, 1.0f});
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(transform));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiDynamicSprite::RenderPicking() const {
  glm::mat3 transform{1.0f};
  transform = glm::translate(
      transform, final_dbg_transform_.translate + local_transform_.translate);
  transform = glm::rotate(
      transform, local_transform_.rotate);
  transform = glm::scale(
      transform, final_dbg_transform_.scale * local_transform_.scale
                     * glm::vec2{gResFactor, 1.0f});
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(transform));
  glUniform1ui(shader::kSpriteId, static_cast<uint32_t>(ui_data_.id));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}


UiStaticSprite::UiStaticSprite(
    vbos::VboIdMain vbo_texture, vbos::VboIdText vbo_text,
    CallableType action)
    : UiCallable(vbo_texture, vbo_text, std::move(action)) {
  UpdateTransform();
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
}

UiStaticSprite::UiStaticSprite(UiStaticSprite&& other) noexcept
    : UiCallable(std::move(other)),
      final_transform_(other.final_transform_) {
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
}

void UiStaticSprite::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  auto parent_dbg_transform = GetParentDbgTransform(ui_data_.id);
  auto dbg_transform =
      debug::gUiTransforms[4 * (ui_data_.id - details::kIdOffsetUi)];
  parent_dbg_transform.Apply(dbg_transform);
  glm::mat3 transform{1.0f};
  transform = glm::translate(transform, parent_dbg_transform.translate);
  transform = glm::scale(transform, parent_dbg_transform.scale
                                        * glm::vec2{gResFactor, 1.0f});
  final_transform_ = transform;
}

void UiStaticSprite::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (ui_data_.id - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

float UiStaticSprite::GetLeftBorder() const {
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
}

float UiStaticSprite::GetRightBorder() const {
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4];
}

float UiStaticSprite::GetTopBorder() const {
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5];
}

float UiStaticSprite::GetBottomBorder() const {
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
}

void UiStaticSprite::Render() {
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(final_transform_));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiStaticSprite::RenderPicking() const {
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(final_transform_));
  glUniform1ui(shader::kSpriteId, static_cast<uint32_t>(ui_data_.id));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

const float UiSliderV::kTrackLengthFactor = 0.8f;

UiSliderV::UiSliderV(
    UiStaticSprite&& fill_sprite,
    UiStaticSprite&& back_sprite,
    UiDynamicSprite&& icon_sprite,
    float scale)
    : UiCallable(back_sprite.ui_data_, {}),
      fill_sprite_(std::move(fill_sprite)),
      back_sprite_(std::move(back_sprite)),
      icon_sprite_(std::move(icon_sprite)),
      length_(kTrackLengthFactor * (back_sprite_.GetTopBorder()
              - back_sprite_.GetBottomBorder())),
      centre_((back_sprite_.GetTopBorder()
               + back_sprite_.GetBottomBorder()) / 2.0f),
      scale_(scale) {
  gUiComponents[fill_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  UpdateTransform();
}

/// to update component features (not transform, but e.g. slider handler,
/// write itself(this) to gUiComponents[id - offset].ui

/// to update transform at the program start get transform from gUiTransforms

/// to set parent-child relationship set gUiComponents->parent_id_

UiSliderV::UiSliderV(UiSliderV&& other) noexcept
    : UiCallable(std::move(other)),
      fill_sprite_(std::move(other.fill_sprite_)),
      back_sprite_(std::move(other.back_sprite_)),
      icon_sprite_(std::move(other.icon_sprite_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  gUiComponents[back_sprite_.ui_data_.id - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

/// we need this only in DEBUG, but in DEBUG we use only UiDynamicSprite,
/// so we don't separate it to RenderPicking and RenderPickingIcon,
/// but not for Render() and RenderPicking()

void UiSliderV::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  back_sprite_.Render();
  glEnable(GL_SCISSOR_TEST);
  // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
  float y_ndc = centre_ - length_ / 2;
  int y_px = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
  int height_px = int(progress_ * length_ * 0.5f * gWindowHeight);
  glScissor(0, y_px, 4000, height_px);

  fill_sprite_.Render();
  glDisable(GL_SCISSOR_TEST);
}

void UiSliderV::RenderIcon() {
  icon_sprite_.Render();
}

void UiSliderV::RenderPicking() const {
  back_sprite_.RenderPicking();
  if (debug::gCtrlMode) {
    glEnable(GL_SCISSOR_TEST);
    // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
    float y_ndc = centre_ - length_ / 2;
    int y_px = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
    int height_px = int(progress_ * length_ * 0.5f * gWindowHeight);
    glScissor(0, y_px, 4000, height_px);
    fill_sprite_.RenderPicking();
    glDisable(GL_SCISSOR_TEST);
    icon_sprite_.RenderPicking();
  }
}

void UiSliderV::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_ / 2.0f;
  float offset = glm::clamp(
      mouse_pos.y - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  glm::vec2 translate = {0.0f, offset};
  icon_sprite_.SetTranslate(translate);
}

size_t UiSliderV::Hover(std::uint32_t id) {
  return back_sprite_.Hover();
}

void UiSliderV::UnHover() {
  //TODO: set lower brightness?
}

float UiSliderV::GetProgress() const {
  //TODO: make some *magic* with sprite and "return progress_;"
  return (1.0f - progress_) * scale_;
}

void UiSliderV::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  length_ = kTrackLengthFactor * scale *
            (back_sprite_.GetTopBorder() - back_sprite_.GetBottomBorder());
  centre_ = y_translate;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({0.0f, related_pos});
  fill_sprite_.UpdateTransform();
  back_sprite_.UpdateTransform();
  icon_sprite_.UpdateTransform();
}

void UiSliderV::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (ui_data_.id - details::kIdOffsetUi)
  ];
  UpdateTransform(transform.translate.x, transform.translate.y,
                  transform.scale);
}


const float UiSliderH::kTrackLengthFactor = 0.7f;

UiSliderH::UiSliderH(
    UiDynamicSprite&& fill_sprite,
    UiDynamicSprite&& back_sprite,
    UiDynamicSprite&& handler_sprite,
    UiDynamicSprite&& icon_sprite,
    float scale)
    : UiCallable(back_sprite.ui_data_, {}),
      fill_sprite_(std::move(fill_sprite)),
      back_sprite_(std::move(back_sprite)),
      icon_sprite_(std::move(icon_sprite)),
      handler_sprite_(std::move(handler_sprite)),
      length_(kTrackLengthFactor * (back_sprite_.GetRightBorder()
              - back_sprite_.GetLeftBorder())),
      centre_((back_sprite_.GetRightBorder()
               + back_sprite_.GetLeftBorder()) / 2.0f),
      scale_(scale) {
  gUiComponents[fill_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[handler_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  std::cout << "+def ctor " << gUiComponents[fill_sprite_.ui_data_.id - details::kIdOffsetUi].parent_id_ << std::endl;
  UpdateTransform();
}

UiSliderH::UiSliderH(UiSliderH&& other) noexcept
    : UiCallable(std::move(other)),
      fill_sprite_(std::move(other.fill_sprite_)),
      back_sprite_(std::move(other.back_sprite_)),
      icon_sprite_(std::move(other.icon_sprite_)),
      handler_sprite_(std::move(other.handler_sprite_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  /// OK, it solves the problem
  gUiComponents[fill_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[handler_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  std::cout << "+move ctor " << gUiComponents[fill_sprite_.ui_data_.id - details::kIdOffsetUi].parent_id_ << std::endl;
  gUiComponents[back_sprite_.ui_data_.id - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

void UiSliderH::SetParentTransform(LocalTransform transform) {
  fill_sprite_.SetParentTransform(transform);
  back_sprite_.SetParentTransform(transform);
  icon_sprite_.SetParentTransform(transform);
  handler_sprite_.SetParentTransform(transform);
}

void UiSliderH::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  back_sprite_.Render();
  glEnable(GL_SCISSOR_TEST);
  // ---- ---- ----
  // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
  float x_ndc = centre_ - length_ / 2;
  int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
  int width_px = int(progress_ * length_ * 0.5f * gWindowWidth);
  glScissor(x_px, 0, width_px, 4000);

  fill_sprite_.Render();
  glDisable(GL_SCISSOR_TEST);

  icon_sprite_.Render();
}

void UiSliderH::RenderIcon() {
  handler_sprite_.Render();
}

void UiSliderH::RenderPicking() const {
  back_sprite_.RenderPicking();
  if (debug::gCtrlMode) {
    glEnable(GL_SCISSOR_TEST);
    // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
    float x_ndc = centre_ - length_ / 2;
    int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
    int width_px = int(progress_ * length_ * 0.5f * gWindowWidth);
    glScissor(x_px, 0, width_px, 4000);
    fill_sprite_.RenderPicking();
    glDisable(GL_SCISSOR_TEST);
    icon_sprite_.RenderPicking();
    handler_sprite_.RenderPicking();
  }
}

void UiSliderH::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_ / 2.0f;
  float offset = glm::clamp(
      mouse_pos.x - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  glm::vec2 translate = {offset, 0.0f};
  handler_sprite_.SetTranslate(translate);
}

size_t UiSliderH::Hover(std::uint32_t id) {
  return back_sprite_.Hover();
}

void UiSliderH::UnHover() {
  //TODO: set lower brightness?
}

float UiSliderH::GetProgress() const {
  //TODO: make some *magic* with sprite and "return progress_;"
  return (1.0f - progress_) * scale_;
}

void UiSliderH::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  length_ = kTrackLengthFactor * scale *
            (back_sprite_.GetRightBorder() - back_sprite_.GetLeftBorder());
  centre_ = x_translate;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({related_pos, 0.0f});
  fill_sprite_.UpdateTransform();
  back_sprite_.UpdateTransform();
  icon_sprite_.UpdateTransform();
  handler_sprite_.UpdateTransform();
}

void UiSliderH::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (ui_data_.id - details::kIdOffsetUi)
  ];
  UpdateTransform(transform.translate.x, transform.translate.y,
                  transform.scale);
}

UiToggle::UiToggle(
    UiStaticSprite&& off, UiStaticSprite&& on1,
    UiStaticSprite&& on2, UiStaticSprite&& on3)
    : UiCallable(off.ui_data_, {}),
      off_(std::move(off)),
      on1_(std::move(on1)),
      on2_(std::move(on2)),
      on3_(std::move(on3)),
      state_(&off_),
      speed_(0.5f) {
  gUiComponents[on1_.GetId() - details::kIdOffsetUi].parent_id_
      = off_.GetId();
  gUiComponents[on2_.GetId() - details::kIdOffsetUi].parent_id_
      = off_.GetId();
  gUiComponents[on3_.GetId() - details::kIdOffsetUi].parent_id_
      = off_.GetId();
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  UpdateTransform();
}

UiToggle::UiToggle(UiToggle&& other) noexcept
    : UiCallable(std::move(other)),
      off_(std::move(other.off_)),
      on1_(std::move(other.on1_)),
      on2_(std::move(other.on3_)),
      on3_(std::move(other.on2_)) {
  progress_ = other.progress_;
  state_ = other.state_;
  turned_off_ = other.turned_off_;
  speed_ = other.speed_;
  progress_ = other.progress_;
  gUiComponents[off_.ui_data_.id - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

void UiToggle::Render() {
  UpdateState();
  state_->Render();
}

void UiToggle::RenderPicking() const {
  off_.RenderPicking();
}

void UiToggle::Press() {
  turned_off_ = !turned_off_;
  progress_ = 0.0f;
  off_.Press();
}

void UiToggle::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  off_.UpdateTransform();
  on1_.UpdateTransform();
  on2_.UpdateTransform();
  on3_.UpdateTransform();
}

void UiToggle::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (ui_data_.id - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

void UiToggle::UpdateState() {
  if (turned_off_) {
    state_ = &off_;
  } else {
    progress_ += speed_ * gDeltaTime;
    // taking fractal part (looping)
    progress_ = progress_ - static_cast<float>(static_cast<int>(progress_));
    if (progress_ < 0.33f) {
      state_ = &on1_;
    } else if (progress_ < 0.67f) {
      state_ = &on2_;
    } else {
      state_ = &on3_;
    }
  }
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
    : UiCallable(sprite0.ui_data_, {}),
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
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
}

UiLoading::UiLoading(UiLoading&& other) noexcept
    : UiCallable(std::move(other)),
      sprites_(std::move(other.sprites_)) {
  gUiComponents[sprites_[0].ui_data_.id - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
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
      4 * (ui_data_.id - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

UiWindowBase::UiWindowBase(
    UiStaticSprite&& sprite, SharedResources& shared_resources)
    : UiCallable(sprite.ui_data_, {}),
      sprite_(std::move(sprite)),
      shared_resources_(shared_resources),
      speed_(4.0f) {}

UiWindowBase::UiWindowBase(UiWindowBase&& other) noexcept
    : UiCallable(std::move(other)),
      sprite_(std::move(other.sprite_)),
      speed_(other.speed_),
      progress_(other.progress_),
      back_ready_(other.back_ready_),
      shared_resources_(other.shared_resources_) {}

/// back_ready_==false when appearing or disappearing animation
/// returs false when disappearing fading is over
bool UiWindowBase::RenderBack(bool show) {
  shared_resources_.mask_sprite_shader_.Bind();
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
  // By default desk texture also in general tex atlas
  //    glActiveTexture(0);
  //    desk_sprite_.Bind();
  glActiveTexture(GL_TEXTURE1);
  shared_resources_.tex_ui_mask_.Bind();
  glUniform1f(shader::kSpriteProgress, progress_);
  sprite_.Render();
  glActiveTexture(GL_TEXTURE0); // go back to default
  return true;
}

void UiWindowBase::RenderPickingBack() {
  shared_resources_.static_sprite_picking_shader_.Bind();
  sprite_.RenderPicking();
}

void UiWindowBase::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (ui_data_.id - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

UiTabMenu::UiTabMenu(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,

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

    UiStaticSprite&& btn_settings,
    UiStaticSprite&& btn_shader_wirebound,

    UiToggle&& toggle_shaders,

    UiDynamicSprite&& cross)
    : Base(std::move(sprite), shared_resources),
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

      btn_settings_(std::move(btn_settings)),
      btn_shader_wirebound_(std::move(btn_shader_wirebound)),
      toggle_shaders_(std::move(toggle_shaders)),
      ui_event_handler_({
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

          &btn_settings_,
          &btn_shader_wirebound_,
          &toggle_shaders_
      }),
      cross_(std::move(cross)) {
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

  gUiComponents[btn_settings_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_shader_wirebound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_shaders_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[cross_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  Base::UpdateTransform();
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

      btn_settings_(std::move(other.btn_settings_)),
      btn_shader_wirebound_(std::move(other.btn_shader_wirebound_)),
      toggle_shaders_(std::move(other.toggle_shaders_)),

      cross_(std::move(other.cross_)),
      ui_event_handler_(std::move(other.ui_event_handler_)) {
  gUiComponents[sprite_.ui_data_.id - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

// returns "stop render"
bool UiTabMenu::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  shared_resources_.static_sprite_shader_.Bind();

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

  btn_settings_.Render();
  btn_shader_wirebound_.Render();

  toggle_shaders_.Render();

  shared_resources_.dynamic_sprite_shader_.Bind();
  cross_.Render();

  return stop_show;
}

void UiTabMenu::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  shared_resources_.static_sprite_picking_shader_.Bind();

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

  btn_settings_.RenderPicking();
  btn_shader_wirebound_.RenderPicking();

  toggle_shaders_.RenderPicking();

  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  cross_.RenderPicking();
}

void UiTabMenu::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  sprite_.UpdateTransform();

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

  btn_settings_.UpdateTransform();
  btn_shader_wirebound_.UpdateTransform();
  toggle_shaders_.UpdateTransform();

  cross_.UpdateTransform();
}

UiSettings::UiSettings(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,
    UiSliderH&& resolution,
    UiSliderH&& music,
    UiSliderH&& sound,
    UiSliderH&& sensitivity,
    UiStaticSprite&& keyboard,
    UiToggle&& toggle_music,
    UiToggle&& toggle_sound,
    UiDynamicSprite&& cross)
    : Base(std::move(sprite), shared_resources),
      resolution_(std::move(resolution)),
      music_(std::move(music)),
      sound_(std::move(sound)),
      sensitivity_(std::move(sensitivity)),
      keyboard_(std::move(keyboard)),
      toggle_music_(std::move(toggle_music)),
      toggle_sound_(std::move(toggle_sound)),
      ui_event_handler_({
          &resolution_, &music_, &sound_, &sensitivity_, &keyboard_,
          &toggle_music_, &toggle_sound_
      }),
      cross_(std::move(cross)) {
  gUiComponents[resolution_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[music_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sensitivity_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[keyboard_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_music_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_sound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[cross_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  Base::UpdateTransform();
}

UiSettings::UiSettings(UiSettings&& other) noexcept
    : Base(std::move(other)),
      resolution_(std::move(other.resolution_)),
      music_(std::move(other.music_)),
      sound_(std::move(other.sound_)),
      sensitivity_(std::move(other.sensitivity_)),
      keyboard_(std::move(other.keyboard_)),
      toggle_music_(std::move(other.toggle_music_)),
      toggle_sound_(std::move(other.toggle_sound_)),
      cross_(std::move(other.cross_)),
      ui_event_handler_(std::move(other.ui_event_handler_)) {
  gUiComponents[sprite_.ui_data_.id - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

// returns "stop render"
bool UiSettings::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  shared_resources_.static_sprite_shader_.Bind();

  auto mouse_pos =
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  resolution_.Render(mouse_pos);
  music_.Render(mouse_pos);
  sound_.Render(mouse_pos);
  sensitivity_.Render(mouse_pos);
  keyboard_.Render();

  toggle_music_.Render();
  toggle_sound_.Render();

  shared_resources_.dynamic_sprite_shader_.Bind();
  cross_.Render();

  return stop_show;
}

void UiSettings::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  shared_resources_.static_sprite_picking_shader_.Bind();

  resolution_.RenderPicking();
  music_.RenderPicking();
  sound_.RenderPicking();
  sensitivity_.RenderPicking();
  keyboard_.RenderPicking();

  toggle_music_.RenderPicking();
  toggle_sound_.RenderPicking();

  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  cross_.RenderPicking();
}

void UiSettings::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  sprite_.UpdateTransform();

  /// children
  resolution_.UpdateTransform();
  music_.UpdateTransform();
  sound_.UpdateTransform();
  sensitivity_.UpdateTransform();
  keyboard_.UpdateTransform();
  toggle_music_.UpdateTransform();
  toggle_sound_.UpdateTransform();
  cross_.UpdateTransform();
}

UiConfirmation::UiConfirmation(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,

    UiStaticSprite&& btn_close,
    UiStaticSprite&& btn_accept,
    UiStaticSprite&& btn_decline,

    UiDynamicSprite&& cross)
    : Base(std::move(sprite), shared_resources),
      btn_close_(std::move(btn_close)),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      ui_event_handler_({
          &btn_close_,
          &btn_accept_,
          &btn_decline_,
      }),
      cross_(std::move(cross)) {
  gUiComponents[btn_close_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_accept_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_decline_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[cross_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  Base::UpdateTransform();
}

UiConfirmation::UiConfirmation(UiConfirmation&& other) noexcept
    : Base(std::move(other)),
      btn_close_(std::move(other.btn_close_)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      cross_(std::move(other.cross_)),
      ui_event_handler_(std::move(other.ui_event_handler_)) {
  gUiComponents[sprite_.ui_data_.id - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

// returns "stop render"
bool UiConfirmation::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  shared_resources_.static_sprite_shader_.Bind();

  btn_close_.Render();
  btn_accept_.Render();
  btn_decline_.Render();

  shared_resources_.dynamic_sprite_shader_.Bind();
  cross_.Render();

  return stop_show;
}

void UiConfirmation::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  shared_resources_.static_sprite_picking_shader_.Bind();

  btn_close_.RenderPicking();
  btn_accept_.RenderPicking();
  btn_decline_.RenderPicking();

  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  cross_.RenderPicking();
}

void UiConfirmation::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  sprite_.UpdateTransform();

  /// children
  btn_close_.UpdateTransform();
  btn_accept_.UpdateTransform();
  btn_decline_.UpdateTransform();
  cross_.UpdateTransform();
}

UiPopUpBase::UiPopUpBase(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,
    LocalTransform start_transform,
    LocalTransform end_transform)
    : UiCallable(sprite.ui_data_, {}),
      sprite_(std::move(sprite)),
      start_transform_(start_transform),
      end_transform_(end_transform),
      shared_resources_(shared_resources),
      speed_(4.0f) {}

UiPopUpBase::UiPopUpBase(UiPopUpBase&& other) noexcept
    : UiCallable(std::move(other)),
      sprite_(std::move(other.sprite_)),
      cur_transform_(other.cur_transform_),
      start_transform_(other.start_transform_),
      end_transform_(other.end_transform_),
      speed_(other.speed_),
      progress_(other.progress_),
      back_ready_(other.back_ready_),
      shared_resources_(other.shared_resources_) {}

bool UiPopUpBase::RenderBack(bool show) {
  shared_resources_.dynamic_sprite_shader_.Bind();
  if (show) {
    progress_ += speed_ * gDeltaTime;
    if (progress_ >= 1.0f) {
      back_ready_ = true;
    }
  } else {
    progress_ -= speed_ * gDeltaTime;
    if (progress_ <= 0.0f) {
      return false;
    }
  }
  progress_ = std::clamp(progress_, 0.0f, 1.0f);
  CubicInterpolation();

  glm::mat3 transform = glm::mat3{1.0f};
  transform = glm::translate(transform, cur_transform_.translate);
  transform = glm::rotate(transform, cur_transform_.rotate);
  transform = glm::scale(transform, glm::vec2{cur_transform_.scale});
  glUniformMatrix3fv(shader::kSpriteTransform, 1, false,
                     glm::value_ptr(transform));

  sprite_.Render();
  return true;
}

void UiPopUpBase::RenderPickingBack() {
  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  glm::mat3 transform = glm::mat3{1.0f};
  transform = glm::translate(transform, cur_transform_.translate);
  transform = glm::rotate(transform, cur_transform_.rotate);
  transform = glm::scale(transform, glm::vec2{cur_transform_.scale});
  glUniformMatrix3fv(shader::kSpriteTransform, 1, false,
                     glm::value_ptr(transform));
  sprite_.RenderPicking();
}

void UiPopUpBase::CubicInterpolation() {
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

void UiPopUpBase::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (ui_data_.id - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

UiWaterLayerConfig::UiWaterLayerConfig(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,
    LocalTransform start_transform,
    LocalTransform end_transform,
    UiDynamicSprite&& sprite_layer,
    UiSliderH&& scale,
    UiSliderH&& fetch,
    UiSliderH&& spread_blend,
    UiSliderH&& swell,
    UiSliderH&& peak_enhancement,
    UiSliderH&& short_waves_fade,
    UiSliderH&& lambda)
    : Base(std::move(sprite), shared_resources,
           start_transform, end_transform),
      sprite_layer_(std::move(sprite_layer)),
      scale_(std::move(scale)),
      fetch_(std::move(fetch)),
      spread_blend_(std::move(spread_blend)),
      swell_(std::move(swell)),
      peak_enhancement_(std::move(peak_enhancement)),
      short_waves_fade_(std::move(short_waves_fade)),
      lambda_(std::move(lambda)),
      ui_event_handler_({
          /*&sprite_layer_, */&scale_, &fetch_, &spread_blend_, &swell_,
          &peak_enhancement_, &short_waves_fade_, &lambda_
      }) {
  gUiComponents[sprite_layer_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[scale_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[fetch_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[spread_blend_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[swell_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[peak_enhancement_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[short_waves_fade_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[lambda_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  ui_data_.ui = static_cast<UiTransformDbg*>(this);
  gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  Base::UpdateTransform();
}

UiWaterLayerConfig::UiWaterLayerConfig(UiWaterLayerConfig&& other) noexcept
    : Base(std::move(other)),
      sprite_layer_(std::move(other.sprite_layer_)),
      scale_(std::move(other.scale_)),
      fetch_(std::move(other.fetch_)),
      spread_blend_(std::move(other.spread_blend_)),
      swell_(std::move(other.swell_)),
      peak_enhancement_(std::move(other.peak_enhancement_)),
      short_waves_fade_(std::move(other.short_waves_fade_)),
      lambda_(std::move(other.lambda_)),
      modified_(other.modified_),
      ui_event_handler_(std::move(other.ui_event_handler_)) {
  gUiComponents[sprite_.ui_data_.id - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

// returns "stop render"
bool UiWaterLayerConfig::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  shared_resources_.dynamic_sprite_shader_.Bind();

  scale_.SetParentTransform(Base::cur_transform_);
  fetch_.SetParentTransform(Base::cur_transform_);
  spread_blend_.SetParentTransform(Base::cur_transform_);
  swell_.SetParentTransform(Base::cur_transform_);
  peak_enhancement_.SetParentTransform(Base::cur_transform_);
  short_waves_fade_.SetParentTransform(Base::cur_transform_);
  lambda_.SetParentTransform(Base::cur_transform_);

  /// shader & transform matrix already bind by Base class
  auto mouse_pos =
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  sprite_layer_.Render();
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

  return stop_show;
}

void UiWaterLayerConfig::Release() {
  scale_.Release();
  fetch_.Release();
  spread_blend_.Release();
  swell_.Release();
  peak_enhancement_.Release();
  short_waves_fade_.Release();
  lambda_.Release();
}

void UiWaterLayerConfig::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sprite_layer_.RenderPicking();
  scale_.RenderPicking();
  fetch_.RenderPicking();
  spread_blend_.RenderPicking();
  swell_.RenderPicking();
  peak_enhancement_.RenderPicking();
  short_waves_fade_.RenderPicking();
  lambda_.RenderPicking();
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
  /// back
  sprite_.UpdateTransform();

  /// children
  sprite_layer_.UpdateTransform();
  scale_.UpdateTransform();
  fetch_.UpdateTransform();
  spread_blend_.UpdateTransform();
  swell_.UpdateTransform();
  peak_enhancement_.UpdateTransform();
  short_waves_fade_.UpdateTransform();
  lambda_.UpdateTransform();
}
