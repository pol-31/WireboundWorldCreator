#include "Ui.h"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "../modes/UiTerrainMode.h"
#include "../modes/UiWaterMode.h"
#include "../common/Colors.h"
#include "../common/ShadersBinding.h"

#include "../common/UiDebugger.h"

//TODO: do we need params for UpdateTransform(
//    float x_translate, float y_translate, float scale)
// ?

std::array<data::UiData, data::gVboIdSize> gUiComponents{};

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

UiBase::UiBase(data::VboIdMain vbo_texture, CallableType action)
    : UiBase(data::GetUiData(vbo_texture), std::move(action)) {}

UiBase::UiBase(data::UiData ui_data, CallableType&& action)
    : ui_data_id_(ui_data.id),
      action_(std::move(action)) {
  gUiComponents[ui_data_id_ - details::kIdOffsetUi] = ui_data;
}

UiBase::UiBase(size_t ui_data_id, CallableType&& action)
    : ui_data_id_(ui_data_id),
      action_(std::move(action)) {}

void UiBase::Press() {
  if (action_) {
    action_();
  }
}

void UiBase::Release() {} // can change everything

bool UiBase::Scroll(GLuint id, float yoffset) {
  return false;
}

std::uint32_t UiBase::GetId() const {
  return ui_data_id_;
}

std::size_t UiBase::GetVboOffset() const {
  return gUiComponents[ui_data_id_ - details::kIdOffsetUi].vbo_offset;
}

UiDynamicSprite::UiDynamicSprite(
    data::VboIdMain vbo_texture, CallableType action)
    : UiBase(vbo_texture, std::move(action)) {
  UpdateTransform();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

UiDynamicSprite::UiDynamicSprite(UiDynamicSprite&& other) noexcept
    : UiBase(std::move(other)),
      local_transform_(other.local_transform_),
      parent_transform_(other.parent_transform_),
      final_dbg_transform_(other.final_dbg_transform_) {
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

void UiDynamicSprite::UpdateTransform() {
  final_dbg_transform_ = GetParentDbgTransform(GetId());
  auto dbg_transform = debug::gUiTransforms[4 * (GetId() - details::kIdOffsetUi)];
  final_dbg_transform_.Apply(dbg_transform);
}

float UiDynamicSprite::GetLeftBorder() const {
  auto scale = final_dbg_transform_.scale
               * local_transform_.scale
               * parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto translate = final_dbg_transform_.translate
                   + local_transform_.translate
                   + parent_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 8] * scale.x + translate.x;
}

float UiDynamicSprite::GetRightBorder() const {
  auto scale = final_dbg_transform_.scale
               * local_transform_.scale
               * parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto translate = final_dbg_transform_.translate
                   + local_transform_.translate
                   + parent_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4] * scale.x + translate.x;
}

float UiDynamicSprite::GetTopBorder() const {
  auto scale = final_dbg_transform_.scale
               * local_transform_.scale
               * parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto translate = final_dbg_transform_.translate
                   + local_transform_.translate
                   + parent_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 5] * scale.y + translate.y;
}

float UiDynamicSprite::GetBottomBorder() const {
  auto scale = final_dbg_transform_.scale
               * local_transform_.scale
               * parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto translate = final_dbg_transform_.translate
                   + local_transform_.translate
                   + parent_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 1] * scale.y + translate.y;
}

void UiDynamicSprite::Render() {
  glm::mat3 transform{1.0f};
  transform = glm::translate(
      transform, final_dbg_transform_.translate + local_transform_.translate
                     + parent_transform_.translate);
  transform = glm::scale(
      transform, final_dbg_transform_.scale * local_transform_.scale
                     * parent_transform_.scale
                     * glm::vec2{gResFactor * extra_scale_, 1.0f});
  transform = glm::rotate(
      transform, local_transform_.rotate + parent_transform_.rotate);
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(transform));
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
}

void UiDynamicSprite::RenderPicking() const {
  glm::mat3 transform{1.0f};
  transform = glm::translate(
      transform, final_dbg_transform_.translate + local_transform_.translate
                     + parent_transform_.translate);
  transform = glm::rotate(
      transform, local_transform_.rotate + parent_transform_.rotate);
  transform = glm::scale(
      transform, final_dbg_transform_.scale * local_transform_.scale
                     * parent_transform_.scale
                     * glm::vec2{gResFactor * extra_scale_, 1.0f});
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(transform));
  glUniform1ui(shader::kSpriteId, static_cast<uint32_t>(GetId()));
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
}


UiStaticSprite::UiStaticSprite(
    data::VboIdMain vbo_texture, CallableType action)
    : UiBase(vbo_texture, std::move(action)) {
  UpdateTransform();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

UiStaticSprite::UiStaticSprite(UiStaticSprite&& other) noexcept
    : UiBase(std::move(other)),
      final_transform_(other.final_transform_) {
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

void UiStaticSprite::UpdateTransform() {
  auto parent_dbg_transform = GetParentDbgTransform(GetId());
  auto dbg_transform =
      debug::gUiTransforms[4 * (GetId() - details::kIdOffsetUi)];
  parent_dbg_transform.Apply(dbg_transform);
  glm::mat3 transform{1.0f};
  transform = glm::translate(transform, parent_dbg_transform.translate);
  transform = glm::scale(transform, parent_dbg_transform.scale
                                        * glm::vec2{gResFactor, 1.0f});
  final_transform_ = transform;
}

float UiStaticSprite::GetLeftBorder() const {
  auto parent_dbg_transform = GetParentDbgTransform(GetId());
  auto dbg_transform =
      debug::gUiTransforms[4 * (GetId() - details::kIdOffsetUi)];
  parent_dbg_transform.Apply(dbg_transform);
  glm::vec2 translate = parent_dbg_transform.translate;
  glm::vec2 scale = parent_dbg_transform.scale * glm::vec2{gResFactor, 1.0f};

  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 8] * scale.x + translate.x;
}

float UiStaticSprite::GetRightBorder() const {
  auto parent_dbg_transform = GetParentDbgTransform(GetId());
  auto dbg_transform =
      debug::gUiTransforms[4 * (GetId() - details::kIdOffsetUi)];
  parent_dbg_transform.Apply(dbg_transform);
  glm::vec2 translate = parent_dbg_transform.translate;
  glm::vec2 scale = parent_dbg_transform.scale * glm::vec2{gResFactor, 1.0f};

  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4] * scale.x + translate.x;
}

float UiStaticSprite::GetTopBorder() const {
  auto parent_dbg_transform = GetParentDbgTransform(GetId());
  auto dbg_transform =
      debug::gUiTransforms[4 * (GetId() - details::kIdOffsetUi)];
  parent_dbg_transform.Apply(dbg_transform);
  glm::vec2 translate = parent_dbg_transform.translate;
  glm::vec2 scale = parent_dbg_transform.scale * glm::vec2{gResFactor, 1.0f};

  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 5] * scale.y + translate.y;
}

float UiStaticSprite::GetBottomBorder() const {
  auto parent_dbg_transform = GetParentDbgTransform(GetId());
  auto dbg_transform =
      debug::gUiTransforms[4 * (GetId() - details::kIdOffsetUi)];
  parent_dbg_transform.Apply(dbg_transform);
  glm::vec2 translate = parent_dbg_transform.translate;
  glm::vec2 scale = parent_dbg_transform.scale * glm::vec2{gResFactor, 1.0f};

  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 1] * scale.y + translate.y;
}

void UiStaticSprite::Render() {
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(final_transform_));
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
}

void UiStaticSprite::RenderPicking() const {
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(final_transform_));
  glUniform1ui(shader::kSpriteId, static_cast<uint32_t>(GetId()));
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
}


UiSpriteTransformation::UiSpriteTransformation(
    UiDynamicSprite& sprite, LocalTransform start, LocalTransform end)
    : sprite_(sprite),
      speed_(2.0f),
      start_(start),
      end_(end) {}

void UiSpriteTransformation::Render() {
  UpdateAnimation();
  sprite_.Render();
}

void UiSpriteTransformation::RenderPicking() {
//  UpdateAnimation(); <- updated in Render()
  sprite_.RenderPicking();
}

void UiSpriteTransformation::RunAnimation(bool looping) {
  progress_ = 0.0f;
  looping_ = looping;
}

void UiSpriteTransformation::StopAnimation() {
  progress_ = 1.0f;
}

void UiSpriteTransformation::SwapStartEnd() {
  auto temp_transform = start_;
  start_ = end_;
  end_ = temp_transform;
}

void UiSpriteTransformation::CubicInterpolation() {
  /*cur_.translate =
      start_.translate + progress_ * (end_.translate - start_.translate);
  cur_.rotate =
      start_.rotate + progress_ * (end_.rotate - start_.rotate);
  cur_.scale =
      start_.scale + progress_ * (end_.scale - start_.scale);
*/

  float t = progress_;
  float cubic_t = t * t * (3.0f - 2.0f * t); // smoothstep

  cur_.translate = start_.translate + cubic_t * (end_.translate - start_.translate);
  cur_.rotate = start_.rotate + cubic_t * (end_.rotate - start_.rotate);
  cur_.scale = start_.scale + cubic_t * (end_.scale - start_.scale);
}

void UiSpriteTransformation::UpdateAnimation() {
  if (progress_ >= 1.0f) {
    return;
  }
  progress_ += speed_ * gDeltaTime;
  if (progress_ >= 1.0f && looping_) {
    SwapStartEnd();
    progress_ = 0.0f;
  }
  progress_ = std::clamp(progress_, 0.0f, 1.0f);
  CubicInterpolation();
  sprite_.SetParentTransform(cur_);
}

const float UiSliderV::kTrackLengthFactor = 0.8f;

UiSliderV::UiSliderV(
    UiDynamicSprite&& fill_sprite,
    UiDynamicSprite&& back_sprite,
    UiDynamicSprite&& icon_sprite,
    float scale)
    : UiBase(back_sprite.GetId(), {}),
      fill_sprite_(std::move(fill_sprite)),
      back_sprite_(std::move(back_sprite)),
      icon_sprite_(std::move(icon_sprite)),
      length_(kTrackLengthFactor *
              (back_sprite_.GetTopBorder()
               - back_sprite_.GetBottomBorder())),
      centre_((back_sprite_.GetTopBorder()
               + back_sprite_.GetBottomBorder()) / 2.0f),
      scale_(scale) {
  gUiComponents[fill_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

/// to update component features (not transform, but e.g. slider handler,
/// write itself(this) to gUiComponents[id - offset].ui

/// to update transform at the program start get transform from gUiTransforms

UiSliderV::UiSliderV(UiSliderV&& other) noexcept
    : UiBase(std::move(other)),
      fill_sprite_(std::move(other.fill_sprite_)),
      back_sprite_(std::move(other.back_sprite_)),
      icon_sprite_(std::move(other.icon_sprite_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  gUiComponents[back_sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
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
  if (debug::gUiAltMode) {
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

void UiSliderV::Set(float progress) {
  progress_ = progress;
  float half_length_ = length_ / 2.0f;
  float offset = progress_ * length_ - half_length_;
  glm::vec2 translate = {0.0f, offset};
  icon_sprite_.SetTranslate(translate);
}

void UiSliderV::SetMouseDiff(
    float remembered_progress,
    glm::vec2 cursor_start, glm::vec2 cursor_end) {
  float diff = (cursor_start.y - cursor_end.y) / gWindowHeight;
  Set(std::clamp(remembered_progress + diff, 0.0f, 1.0f));
}

bool UiSliderV::Scroll(GLuint id, float yoffset) {
  if (id > icon_sprite_.GetId() || id < fill_sprite_.GetId()) {
    return false;
  }
  float factor = 0.01f * yoffset;
  float progress = std::clamp(progress_ + factor, 0.0f, 1.0f);
  Set(progress);
  return true;
}

float UiSliderV::GetProgress() const {
  //TODO: make some *magic* with sprite and "return progress_;"
  return (1.0f - progress_) * scale_;
}

float UiSliderV::GetProgressUnscaled() const {
  return 1.0f - progress_;
}

void UiSliderV::UpdateTransform() {
  back_sprite_.UpdateTransform();
  fill_sprite_.UpdateTransform();
  icon_sprite_.UpdateTransform();
  length_ = kTrackLengthFactor *
            (back_sprite_.GetTopBorder() - back_sprite_.GetBottomBorder());
  centre_ = (back_sprite_.GetTopBorder() + back_sprite_.GetBottomBorder()) / 2.0f;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({0.0f, related_pos});
}

void UiSliderV::SetParentTransform(LocalTransform transform) {
  back_sprite_.SetParentTransform(transform);
  fill_sprite_.SetParentTransform(transform);
  icon_sprite_.SetParentTransform(transform);
}


const float UiSliderH::kTrackLengthFactor = 0.8f;

UiSliderH::UiSliderH(
    UiDynamicSprite&& fill_sprite,
    UiDynamicSprite&& back_sprite,
    UiDynamicSprite&& handler_sprite,
    float scale)
    : UiBase(back_sprite.GetId(), {}),
      fill_sprite_(std::move(fill_sprite)),
      back_sprite_(std::move(back_sprite)),
      handler_sprite_(std::move(handler_sprite)),
      length_(kTrackLengthFactor * (back_sprite_.GetRightBorder()
              - back_sprite_.GetLeftBorder())),
      centre_((back_sprite_.GetRightBorder()
               + back_sprite_.GetLeftBorder()) / 2.0f),
      scale_(scale) {
  gUiComponents[fill_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[handler_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiSliderH::UiSliderH(UiSliderH&& other) noexcept
    : UiBase(std::move(other)),
      fill_sprite_(std::move(other.fill_sprite_)),
      back_sprite_(std::move(other.back_sprite_)),
      handler_sprite_(std::move(other.handler_sprite_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  gUiComponents[back_sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiSliderH::SetParentTransform(LocalTransform transform) {
  back_sprite_.SetParentTransform(transform);
  fill_sprite_.SetParentTransform(transform);
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
}

void UiSliderH::RenderIcon() {
  handler_sprite_.Render();
}

void UiSliderH::RenderPicking() const {
  back_sprite_.RenderPicking();
  if (debug::gUiAltMode) {
    glEnable(GL_SCISSOR_TEST);
    // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
    float x_ndc = centre_ - length_ / 2;
    int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
    int width_px = int(progress_ * length_ * 0.5f * gWindowWidth);
    glScissor(x_px, 0, width_px, 4000);
    fill_sprite_.RenderPicking();
    glDisable(GL_SCISSOR_TEST);
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

void UiSliderH::Set(float progress) {
  progress_ = progress;
  float half_length_ = length_ / 2.0f;
  float offset = progress_ * length_ - half_length_;
  glm::vec2 translate = {offset, 0.0f};
  handler_sprite_.SetTranslate(translate);
}

bool UiSliderH::Scroll(GLuint id, float yoffset) {
  if (id > handler_sprite_.GetId() || id < fill_sprite_.GetId()) {
    return false;
  }
  float factor = 0.01f * yoffset;
  float progress = std::clamp(progress_ + factor, 0.0f, 1.0f);
  Set(progress);
  return true;
}

float UiSliderH::GetProgress() const {
  //TODO: make some *magic* with sprite and "return progress_;"
  return (1.0f - progress_) * scale_;
}

void UiSliderH::UpdateTransform() {
  back_sprite_.UpdateTransform();
  fill_sprite_.UpdateTransform();
  handler_sprite_.UpdateTransform();
  length_ = kTrackLengthFactor *
            (back_sprite_.GetRightBorder() - back_sprite_.GetLeftBorder());
  centre_ = (back_sprite_.GetRightBorder() + back_sprite_.GetLeftBorder()) / 2.0f;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({related_pos, 0.0f});
}


const float UiSliderH3::kTrackLengthFactor = 0.9f;

UiSliderH3::UiSliderH3(
    UiDynamicSprite&& fill_sprite,
    UiDynamicSprite&& back_sprite,
    UiDynamicSprite&& icon_sprite,
    float scale)
    : UiBase(back_sprite.GetId(), {}),
      fill_sprite_(std::move(fill_sprite)),
      back_sprite_(std::move(back_sprite)),
      icon_sprite_(std::move(icon_sprite)),
      length_(kTrackLengthFactor *
              (back_sprite_.GetRightBorder()
               - back_sprite_.GetLeftBorder())),
      centre_((back_sprite_.GetRightBorder()
               + back_sprite_.GetLeftBorder()) / 2.0f),
      scale_(scale) {
  gUiComponents[fill_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiSliderH3::UiSliderH3(UiSliderH3&& other) noexcept
    : UiBase(std::move(other)),
      fill_sprite_(std::move(other.fill_sprite_)),
      back_sprite_(std::move(other.back_sprite_)),
      icon_sprite_(std::move(other.icon_sprite_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  gUiComponents[back_sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiSliderH3::SetParentTransform(LocalTransform transform) {
  back_sprite_.SetParentTransform(transform);
  fill_sprite_.SetParentTransform(transform);
  icon_sprite_.SetParentTransform(transform);
}

void UiSliderH3::Render(glm::vec2 mouse_pos) {
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
}

void UiSliderH3::RenderIcon() {
  icon_sprite_.Render();
}

void UiSliderH3::RenderPicking() const {
  back_sprite_.RenderPicking();
  if (debug::gUiAltMode) {
    glEnable(GL_SCISSOR_TEST);
    // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
    float x_ndc = centre_ - length_ / 2;
    int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
    int width_px = int(progress_ * length_ * 0.5f * gWindowWidth);
    glScissor(x_px, 0, width_px, 4000);
    fill_sprite_.RenderPicking();
    glDisable(GL_SCISSOR_TEST);
    icon_sprite_.RenderPicking();
  }
}

void UiSliderH3::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_ / 2.0f;
  float offset = glm::clamp(
      mouse_pos.x - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  glm::vec2 translate = {offset, 0.0f};
  icon_sprite_.SetTranslate(translate);
}

void UiSliderH3::Set(float progress) {
  progress_ = progress;
  float half_length_ = length_ / 2.0f;
  float offset = progress_ * length_ - half_length_;
  glm::vec2 translate = {offset, 0.0f};
  icon_sprite_.SetTranslate(translate);
}

bool UiSliderH3::Scroll(GLuint id, float yoffset) {
  if (id > icon_sprite_.GetId() || id < fill_sprite_.GetId()) {
    return false;
  }
  float factor = 0.01f * yoffset;
  float progress = std::clamp(progress_ + factor, 0.0f, 1.0f);
  Set(progress);
  return true;
}

float UiSliderH3::GetProgress() const {
  //TODO: make some *magic* with sprite and "return progress_;"
  return (1.0f - progress_) * scale_;
}

void UiSliderH3::UpdateTransform() {
  back_sprite_.UpdateTransform();
  fill_sprite_.UpdateTransform();
  icon_sprite_.UpdateTransform();
  length_ = kTrackLengthFactor *
            (back_sprite_.GetRightBorder() - back_sprite_.GetLeftBorder());
  centre_ = (back_sprite_.GetRightBorder() + back_sprite_.GetLeftBorder()) / 2.0f;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({related_pos, 0.0f});
}


const float UiSliderH2::kTrackLengthFactor = 0.9f;

UiSliderH2::UiSliderH2(
    UiDynamicSprite&& back_sprite,
    UiDynamicSprite&& icon_sprite,
    float scale)
    : UiBase(back_sprite.GetId(), {}),
      back_sprite_(std::move(back_sprite)),
      icon_sprite_(std::move(icon_sprite)),
      length_(kTrackLengthFactor *
              (back_sprite_.GetRightBorder()
               - back_sprite_.GetLeftBorder())),
      centre_((back_sprite_.GetRightBorder()
               + back_sprite_.GetLeftBorder()) / 2.0f),
      scale_(scale) {
  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiSliderH2::UiSliderH2(UiSliderH2&& other) noexcept
    : UiBase(std::move(other)),
      back_sprite_(std::move(other.back_sprite_)),
      icon_sprite_(std::move(other.icon_sprite_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  gUiComponents[back_sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiSliderH2::SetParentTransform(LocalTransform transform) {
  back_sprite_.SetParentTransform(transform);
  icon_sprite_.SetParentTransform(transform);
}

void UiSliderH2::Render(glm::vec2 mouse_pos) {
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
  glDisable(GL_SCISSOR_TEST);
  icon_sprite_.Render();
}

void UiSliderH2::RenderIcon() {
  icon_sprite_.Render();
}

void UiSliderH2::RenderPicking() const {
  back_sprite_.RenderPicking();
  if (debug::gUiAltMode) {
    glEnable(GL_SCISSOR_TEST);
    // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
    float x_ndc = centre_ - length_ / 2;
    int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
    int width_px = int(progress_ * length_ * 0.5f * gWindowWidth);
    glScissor(x_px, 0, width_px, 4000);
    glDisable(GL_SCISSOR_TEST);
    icon_sprite_.RenderPicking();
  }
}

void UiSliderH2::SetValue(float value) {
  /// deduced from GetProgress
  Set(1.0f - value / scale_);
}


void UiSliderH2::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_ / 2.0f;
  float offset = glm::clamp(
      mouse_pos.x - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  glm::vec2 translate = {offset, 0.0f};
  icon_sprite_.SetTranslate(translate);
}

void UiSliderH2::Set(float progress) {
  progress_ = progress;
  float half_length_ = length_ / 2.0f;
  float offset = progress_ * length_ - half_length_;
  glm::vec2 translate = {offset, 0.0f};
  icon_sprite_.SetTranslate(translate);
}

void UiSliderH2::Press() {
  pressed_ = true;
}

void UiSliderH2::Release() {
  pressed_ = false;
}

bool UiSliderH2::Scroll(GLuint id, float yoffset) {
  if (id > icon_sprite_.GetId() || id < back_sprite_.GetId()) {
    return false;
  }
  float factor = 0.01f * yoffset;
  float progress = std::clamp(progress_ + factor, 0.0f, 1.0f);
  Set(progress);
  return true;
}

float UiSliderH2::GetProgress() const {
  //TODO: make some *magic* with sprite and "return progress_;"
  return (1.0f - progress_) * scale_;
}

void UiSliderH2::UpdateTransform() {
  back_sprite_.UpdateTransform();
  icon_sprite_.UpdateTransform();
  length_ = kTrackLengthFactor *
            (back_sprite_.GetRightBorder() - back_sprite_.GetLeftBorder());
  centre_ = (back_sprite_.GetRightBorder() + back_sprite_.GetLeftBorder()) / 2.0f;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({related_pos, 0.0f});
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

void UiSlider2D::UpdateTransform() {
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

void UiSlider2D::SetParentTransform(LocalTransform transform) {
  palette_.SetParentTransform(transform);
  cursor_.SetParentTransform(transform);
}

UiToggle::UiToggle(
    UiDynamicSprite&& off, UiDynamicSprite&& on1,
    UiDynamicSprite&& on2, UiDynamicSprite&& on3)
    : UiBase(off.GetId(), {}),
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
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiToggle::UiToggle(UiToggle&& other) noexcept
    : UiBase(std::move(other)),
      off_(std::move(other.off_)),
      on1_(std::move(other.on1_)),
      on2_(std::move(other.on3_)),
      on3_(std::move(other.on2_)) {
  progress_ = other.progress_;
  state_ = other.state_;
  turned_off_ = other.turned_off_;
  speed_ = other.speed_;
  progress_ = other.progress_;
  gUiComponents[off_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
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

void UiToggle::UpdateTransform() {
  off_.UpdateTransform();
  on1_.UpdateTransform();
  on2_.UpdateTransform();
  on3_.UpdateTransform();
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

void UiToggle::SetParentTransform(LocalTransform transform) {
  off_.SetParentTransform(transform);
  on1_.SetParentTransform(transform);
  on2_.SetParentTransform(transform);
  on3_.SetParentTransform(transform);
}

void UiToggle::SetTranslate(glm::vec2 translate) {
  off_.SetTranslate(translate);
  on1_.SetTranslate(translate);
  on2_.SetTranslate(translate);
  on3_.SetTranslate(translate);
}

UiToggle2::UiToggle2(
    UiDynamicSprite&& off, UiDynamicSprite&& on)
    : UiBase(off.GetId(), {}),
      off_(std::move(off)),
      on_(std::move(on))  {
  gUiComponents[on_.GetId() - details::kIdOffsetUi].parent_id_
      = off_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiToggle2::UiToggle2(UiToggle2&& other) noexcept
    : UiBase(std::move(other)),
      off_(std::move(other.off_)),
      on_(std::move(other.on_)),
      turned_off_(other.turned_off_) {
  gUiComponents[off_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiToggle2::Render() {
  off_.Render();
  if (!turned_off_) {
    on_.Render();
  }
}

void UiToggle2::RenderPicking() const {
  off_.RenderPicking();
  if (debug::gUiAltMode) {
    on_.RenderPicking();
  }
}

void UiToggle2::Press() {
  turned_off_ = !turned_off_;
  off_.Press();
}

void UiToggle2::UpdateTransform() {
  off_.UpdateTransform();
  on_.UpdateTransform();
}

void UiToggle2::SetParentTransform(LocalTransform transform) {
  off_.SetParentTransform(transform);
  on_.SetParentTransform(transform);
}

void UiToggle2::SetTranslate(glm::vec2 translate) {
  off_.SetTranslate(translate);
  on_.SetTranslate(translate);
}
