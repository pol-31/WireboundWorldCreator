#include "Ui.h"

#include <cmath>
#include <glm/gtc/type_ptr.hpp>

#include "../common/UiDebugger.h"
#include "../modes/UiTerrainMode.h"
#include "../modes/UiWaterMode.h"

// TODO: do we need params for UpdateTransform(
//     float x_translate, float y_translate, float scale)
//  ?

std::array<data::UiData, data::gVboIdSize> gUiComponents{};

LocalTransformLinear GetParentDbgTransform(size_t id) {
  auto parent_id = gUiComponents[id - details::kIdOffsetUi].parent_id_;
  LocalTransformLinear transform{};
  if (parent_id != 0) {
    transform = debug::gUiTransforms[4 * (parent_id - details::kIdOffsetUi)];
    transform.Apply(GetParentDbgTransform(parent_id));
  }
  return transform;
}

UiBase::UiBase(data::VboIdMain vbo_texture, CallableType action)
    : UiBase(data::GetUiData(vbo_texture), std::move(action)) {}

UiBase::UiBase(data::UiData ui_data, CallableType&& action)
    : ui_data_id_(ui_data.id), action_(std::move(action)) {
  gUiComponents[ui_data_id_ - details::kIdOffsetUi] = ui_data;
}

UiBase::UiBase(size_t ui_data_id, CallableType&& action)
    : ui_data_id_(ui_data_id), action_(std::move(action)) {}

void UiBase::Press() {
  if (action_) {
    action_();
  }
}

void UiBase::Release() {}  // can change everything

bool UiBase::Scroll(GLuint id, float yoffset) { return false; }

std::uint32_t UiBase::GetId() const { return ui_data_id_; }

std::size_t UiBase::GetVboOffset() const {
  return gUiComponents[ui_data_id_ - details::kIdOffsetUi].vbo_offset;
}

UiDynamicSprite::UiDynamicSprite(data::VboIdMain vbo_texture,
                                 CallableType action)
    : UiBase(vbo_texture, std::move(action)) {
  UpdateTransform();
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
}

UiDynamicSprite::UiDynamicSprite(UiDynamicSprite&& other) noexcept
    : UiBase(std::move(other)),
      local_transform_(other.local_transform_),
      parent_transform_(other.parent_transform_),
      final_dbg_transform_(other.final_dbg_transform_) {
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
}

void UiDynamicSprite::UpdateTransform() {
  final_dbg_transform_ = GetParentDbgTransform(GetId());
  auto dbg_transform =
      debug::gUiTransforms[4 * (GetId() - details::kIdOffsetUi)];
  final_dbg_transform_.Apply(dbg_transform);
}

float UiDynamicSprite::GetLeftBorder() const {
  auto scale = final_dbg_transform_.scale * local_transform_.scale *
               parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto translate = final_dbg_transform_.translate + local_transform_.translate +
                   parent_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 8] * scale.x + translate.x;
}

float UiDynamicSprite::GetRightBorder() const {
  auto scale = final_dbg_transform_.scale * local_transform_.scale *
               parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto translate = final_dbg_transform_.translate + local_transform_.translate +
                   parent_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4] * scale.x + translate.x;
}

float UiDynamicSprite::GetTopBorder() const {
  auto scale = final_dbg_transform_.scale * local_transform_.scale *
               parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto translate = final_dbg_transform_.translate + local_transform_.translate +
                   parent_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 5] * scale.y + translate.y;
}

float UiDynamicSprite::GetBottomBorder() const {
  auto scale = final_dbg_transform_.scale * local_transform_.scale *
               parent_transform_.scale * glm::vec2{gResFactor, 1.0f};
  auto translate = final_dbg_transform_.translate + local_transform_.translate +
                   parent_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 1] * scale.y + translate.y;
}

float UiDynamicSprite::GetHeight() const {
  return GetTopBorder() - GetBottomBorder();
}

float UiDynamicSprite::GetWidth() const {
  return GetRightBorder() - GetLeftBorder();
}

glm::vec2 UiDynamicSprite::GetCentre() const {
  return {
      (GetLeftBorder() + GetRightBorder()) / 2.0f,
      (GetTopBorder() + GetBottomBorder()) / 2.0f,
  };
}

float UiDynamicSprite::GetLeftBorderUnScaled() const {
  auto scale = final_dbg_transform_.scale * local_transform_.scale *
               parent_transform_.scale;
  auto translate = final_dbg_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 8] + translate.x;
}

float UiDynamicSprite::GetRightBorderUnScaled() const {
  auto translate = final_dbg_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4] + translate.x;
}

float UiDynamicSprite::GetTopBorderUnScaled() const {
  auto translate = final_dbg_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 5] + translate.y;
}

float UiDynamicSprite::GetBottomBorderUnScaled() const {
  auto scale = final_dbg_transform_.scale * local_transform_.scale *
               parent_transform_.scale;
  auto translate = final_dbg_transform_.translate;
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  return data::kUiVboDataMain[vbo_offset * 4 + 1] + translate.y;
}

void UiDynamicSprite::Render() {
  glm::mat3 transform{1.0f};
  transform = glm::translate(transform, final_dbg_transform_.translate +
                                            local_transform_.translate +
                                            parent_transform_.translate);
  transform = glm::scale(transform,
                         final_dbg_transform_.scale * local_transform_.scale *
                             parent_transform_.scale *
                             glm::vec2{gResFactor * extra_scale_, 1.0f});
  transform = glm::rotate(transform,
                          local_transform_.rotate + parent_transform_.rotate);
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(transform));
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
}

void UiDynamicSprite::RenderPicking() const {
  glm::mat3 transform{1.0f};
  transform = glm::translate(transform, final_dbg_transform_.translate +
                                            local_transform_.translate +
                                            parent_transform_.translate);
  transform = glm::rotate(transform,
                          local_transform_.rotate + parent_transform_.rotate);
  transform = glm::scale(transform,
                         final_dbg_transform_.scale * local_transform_.scale *
                             parent_transform_.scale *
                             glm::vec2{gResFactor * extra_scale_, 1.0f});
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(transform));
  glUniform1ui(1, static_cast<uint32_t>(GetId()));
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
}

UiStaticSprite::UiStaticSprite(data::VboIdMain vbo_texture, CallableType action)
    : UiBase(vbo_texture, std::move(action)) {
  UpdateTransform();
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
}

UiStaticSprite::UiStaticSprite(UiStaticSprite&& other) noexcept
    : UiBase(std::move(other)), final_transform_(other.final_transform_) {
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
}

void UiStaticSprite::UpdateTransform() {
  auto parent_dbg_transform = GetParentDbgTransform(GetId());
  auto dbg_transform =
      debug::gUiTransforms[4 * (GetId() - details::kIdOffsetUi)];
  parent_dbg_transform.Apply(dbg_transform);
  glm::mat3 transform{1.0f};
  transform = glm::translate(transform, parent_dbg_transform.translate);
  transform = glm::scale(
      transform, parent_dbg_transform.scale * glm::vec2{gResFactor, 1.0f});
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
  glUniform1ui(1, static_cast<uint32_t>(GetId()));
  auto vbo_offset = gUiComponents[GetId() - details::kIdOffsetUi].vbo_offset;
  glDrawArrays(GL_TRIANGLE_STRIP, vbo_offset, 4);
}

UiSpriteTransformation::UiSpriteTransformation(UiDynamicSprite& sprite,
                                               LocalTransform start,
                                               LocalTransform end)
    : sprite_(sprite), speed_(2.0f), start_(start), end_(end) {}

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

void UiSpriteTransformation::StopAnimation() { progress_ = 1.0f; }

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
  float cubic_t = t * t * (3.0f - 2.0f * t);  // smoothstep

  cur_.translate =
      start_.translate + cubic_t * (end_.translate - start_.translate);
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

const float UiSliderV3::kTrackLengthFactor = 0.8f;

UiSliderV3::UiSliderV3(UiDynamicSprite&& sp_fill, UiDynamicSprite&& sp_track,
                       UiDynamicSprite&& sp_handle, float scale)
    : UiBase(sp_track.GetId(), {}),
      sp_fill_(std::move(sp_fill)),
      sp_track_(std::move(sp_track)),
      sp_handle_(std::move(sp_handle)),
      length_(kTrackLengthFactor *
              (sp_track_.GetTopBorder() - sp_track_.GetBottomBorder())),
      centre_((sp_track_.GetTopBorder() + sp_track_.GetBottomBorder()) / 2.0f),
      scale_(scale) {
  gUiComponents[sp_fill_.GetId() - details::kIdOffsetUi].parent_id_ =
      sp_track_.GetId();
  gUiComponents[sp_handle_.GetId() - details::kIdOffsetUi].parent_id_ =
      sp_track_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
  UpdateTransform();
}

UiSliderV3::UiSliderV3(UiSliderV3&& other) noexcept
    : UiBase(std::move(other)),
      sp_fill_(std::move(other.sp_fill_)),
      sp_track_(std::move(other.sp_track_)),
      sp_handle_(std::move(other.sp_handle_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  gUiComponents[sp_track_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

void UiSliderV3::SetParentTransform(LocalTransform transform) {
  sp_track_.SetParentTransform(transform);
  sp_fill_.SetParentTransform(transform);
  sp_handle_.SetParentTransform(transform);
}

void UiSliderV3::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  sp_track_.Render();
  glEnable(GL_SCISSOR_TEST);
  float y_ndc = centre_ - length_ / 2;
  int y_px = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
  int height_px = int(progress_ * length_ * 0.5f * gWindowHeight);
  glScissor(0, y_px, gWindowWidth, height_px);
  sp_fill_.Render();
  glDisable(GL_SCISSOR_TEST);
  sp_handle_.Render();
}

void UiSliderV3::RenderPicking() const {
  sp_track_.RenderPicking();
  if (debug::gUiAltMode) {
    glEnable(GL_SCISSOR_TEST);
    float y_ndc = centre_ - length_ / 2;
    int y_px = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
    int height_px = int(progress_ * length_ * 0.5f * gWindowHeight);
    glScissor(0, y_px, gWindowWidth, height_px);
    sp_fill_.RenderPicking();
    glDisable(GL_SCISSOR_TEST);
    sp_handle_.RenderPicking();
  }
}

void UiSliderV3::Press() { pressed_ = true; }

void UiSliderV3::Release() { pressed_ = false; }

void UiSliderV3::SetValue(float value) { Set(value / scale_); }

void UiSliderV3::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_ / 2.0f;
  float offset =
      glm::clamp(mouse_pos.y - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  glm::vec2 translate = {0.0f, offset};
  sp_handle_.SetTranslate(translate);
}

void UiSliderV3::Set(float progress) {
  progress_ = std::clamp(progress, 0.0f, 1.0f);
  float half_length_ = length_ / 2.0f;
  float offset = progress_ * length_ - half_length_;
  glm::vec2 translate = {0.0f, offset};
  sp_handle_.SetTranslate(translate);
}

bool UiSliderV3::Scroll(GLuint id, float yoffset) {
  if (id > sp_handle_.GetId() || id < sp_fill_.GetId()) {
    return false;
  }
  Set(progress_ + yoffset * 0.01f);
  return true;
}

float UiSliderV3::GetProgress() const { return progress_ * scale_; }

void UiSliderV3::UpdateTransform() {
  sp_track_.UpdateTransform();
  sp_fill_.UpdateTransform();
  sp_handle_.UpdateTransform();
  length_ = kTrackLengthFactor *
            (sp_track_.GetTopBorder() - sp_track_.GetBottomBorder());
  centre_ = (sp_track_.GetTopBorder() + sp_track_.GetBottomBorder()) / 2.0f;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({0.0f, related_pos});
}

void UiSliderV3::SetMouseDiff(float prev_progress, glm::vec2 start,
                              glm::vec2 end) {
  float diff = (start.y - end.y) / gWindowHeight;
  SetValue(prev_progress + diff * scale_);
}

const float UiSliderH3::kTrackLengthFactor = 0.9f;

UiSliderH3::UiSliderH3(UiDynamicSprite&& sp_fill, UiDynamicSprite&& sp_track,
                       UiDynamicSprite&& sp_handle, float scale)
    : UiBase(sp_track.GetId(), {}),
      sp_fill_(std::move(sp_fill)),
      sp_track_(std::move(sp_track)),
      sp_handle_(std::move(sp_handle)),
      length_(kTrackLengthFactor *
              (sp_track_.GetRightBorder() - sp_track_.GetLeftBorder())),
      centre_((sp_track_.GetRightBorder() + sp_track_.GetLeftBorder()) / 2.0f),
      scale_(scale) {
  gUiComponents[sp_fill_.GetId() - details::kIdOffsetUi].parent_id_ =
      sp_track_.GetId();
  gUiComponents[sp_handle_.GetId() - details::kIdOffsetUi].parent_id_ =
      sp_track_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
  UpdateTransform();
}

UiSliderH3::UiSliderH3(UiSliderH3&& other) noexcept
    : UiBase(std::move(other)),
      sp_fill_(std::move(other.sp_fill_)),
      sp_track_(std::move(other.sp_track_)),
      sp_handle_(std::move(other.sp_handle_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  gUiComponents[sp_track_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

void UiSliderH3::SetParentTransform(LocalTransform transform) {
  sp_track_.SetParentTransform(transform);
  sp_fill_.SetParentTransform(transform);
  sp_handle_.SetParentTransform(transform);
}

void UiSliderH3::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  sp_track_.Render();
  glEnable(GL_SCISSOR_TEST);
  float x_ndc = centre_ - length_ / 2;
  int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
  int width_px = int(progress_ * length_ * 0.5f * gWindowWidth);
  glScissor(x_px, 0, width_px, gWindowHeight);
  sp_fill_.Render();
  glDisable(GL_SCISSOR_TEST);
  sp_handle_.Render();
}

void UiSliderH3::RenderPicking() const {
  sp_track_.RenderPicking();
  if (debug::gUiAltMode) {
    glEnable(GL_SCISSOR_TEST);
    float x_ndc = centre_ - length_ / 2;
    int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
    int width_px = int(progress_ * length_ * 0.5f * gWindowWidth);
    glScissor(x_px, 0, width_px, gWindowHeight);
    sp_fill_.RenderPicking();
    glDisable(GL_SCISSOR_TEST);
    sp_handle_.RenderPicking();
  }
}

void UiSliderH3::Press() { pressed_ = true; }

void UiSliderH3::Release() { pressed_ = false; }

void UiSliderH3::SetValue(float value) { Set(value / scale_); }

void UiSliderH3::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_ / 2.0f;
  float offset =
      glm::clamp(mouse_pos.x - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  glm::vec2 translate = {offset, 0.0f};
  sp_handle_.SetTranslate(translate);
}

void UiSliderH3::Set(float progress) {
  progress_ = std::clamp(progress, 0.0f, 1.0f);
  float half_length_ = length_ / 2.0f;
  float offset = progress_ * length_ - half_length_;
  glm::vec2 translate = {offset, 0.0f};
  sp_handle_.SetTranslate(translate);
}

bool UiSliderH3::Scroll(GLuint id, float yoffset) {
  if (id > sp_handle_.GetId() || id < sp_fill_.GetId()) {
    return false;
  }
  Set(progress_ + yoffset * 0.01f);
  return true;
}

float UiSliderH3::GetProgress() const { return progress_ * scale_; }

void UiSliderH3::UpdateTransform() {
  sp_track_.UpdateTransform();
  sp_fill_.UpdateTransform();
  sp_handle_.UpdateTransform();
  length_ = kTrackLengthFactor *
            (sp_track_.GetRightBorder() - sp_track_.GetLeftBorder());
  centre_ = (sp_track_.GetRightBorder() + sp_track_.GetLeftBorder()) / 2.0f;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({related_pos, 0.0f});
}

const float UiSliderH2::kTrackLengthFactor = 0.9f;

UiSliderH2::UiSliderH2(UiDynamicSprite&& sp_track, UiDynamicSprite&& sp_handle,
                       float scale)
    : UiBase(sp_track.GetId(), {}),
      sp_track_(std::move(sp_track)),
      sp_handle_(std::move(sp_handle)),
      length_(kTrackLengthFactor *
              (sp_track_.GetRightBorder() - sp_track_.GetLeftBorder())),
      centre_((sp_track_.GetRightBorder() + sp_track_.GetLeftBorder()) / 2.0f),
      scale_(scale) {
  gUiComponents[sp_handle_.GetId() - details::kIdOffsetUi].parent_id_ =
      sp_track_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
  UpdateTransform();
}

UiSliderH2::UiSliderH2(UiSliderH2&& other) noexcept
    : UiBase(std::move(other)),
      sp_track_(std::move(other.sp_track_)),
      sp_handle_(std::move(other.sp_handle_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  scale_ = other.scale_;
  gUiComponents[sp_track_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

void UiSliderH2::SetParentTransform(LocalTransform transform) {
  sp_track_.SetParentTransform(transform);
  sp_handle_.SetParentTransform(transform);
}

void UiSliderH2::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  sp_track_.Render();
  sp_handle_.Render();
}

void UiSliderH2::RenderPicking() const {
  sp_track_.RenderPicking();
  if (debug::gUiAltMode) {
    sp_handle_.RenderPicking();
  }
}

void UiSliderH2::Press() { pressed_ = true; }

void UiSliderH2::Release() { pressed_ = false; }

void UiSliderH2::SetValue(float value) { Set(value / scale_); }

void UiSliderH2::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_ / 2.0f;
  float offset =
      glm::clamp(mouse_pos.x - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  glm::vec2 translate = {offset, 0.0f};
  sp_handle_.SetTranslate(translate);
}

void UiSliderH2::Set(float progress) {
  progress_ = std::clamp(progress, 0.0f, 1.0f);
  float half_length_ = length_ / 2.0f;
  float offset = progress_ * length_ - half_length_;
  glm::vec2 translate = {offset, 0.0f};
  sp_handle_.SetTranslate(translate);
}

bool UiSliderH2::Scroll(GLuint id, float yoffset) {
  if (id > sp_handle_.GetId() || id < sp_track_.GetId()) {
    return false;
  }
  Set(progress_ + yoffset * 0.01f);
  return true;
}

void UiSliderH2::SetTranslate(glm::vec2 translate) {
  sp_track_.SetTranslate(translate);
  auto parent_transform = sp_track_.GetParentTransform();
  parent_transform.Apply(sp_track_.GetTransform());
  sp_handle_.SetParentTransform(parent_transform);
}

float UiSliderH2::GetProgress() const { return progress_ * scale_; }

void UiSliderH2::UpdateTransform() {
  sp_track_.UpdateTransform();
  sp_handle_.UpdateTransform();
  length_ = kTrackLengthFactor *
            (sp_track_.GetRightBorder() - sp_track_.GetLeftBorder());
  centre_ = (sp_track_.GetRightBorder() + sp_track_.GetLeftBorder()) / 2.0f;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({related_pos, 0.0f});
}

const float UiSlider2D::kTrackWidthFactor = 0.9f;
const float UiSlider2D::kTrackHeightFactor = 0.8f;

UiSlider2D::UiSlider2D(UiDynamicSprite&& sp_palette,
                       UiDynamicSprite&& sp_handle, glm::vec2 scale)
    : UiBase(sp_palette.GetId(), {}),
      sp_track_(std::move(sp_palette)),
      sp_handle_(std::move(sp_handle)),
      length_({kTrackWidthFactor *
                   (sp_palette.GetRightBorder() - sp_palette.GetLeftBorder()),
               kTrackHeightFactor *
                   (sp_palette.GetTopBorder() - sp_palette.GetBottomBorder())}),
      centre_(
          {(sp_palette.GetRightBorder() + sp_palette.GetLeftBorder()) / 2.0f,
           (sp_palette.GetTopBorder() + sp_palette.GetBottomBorder()) / 2.0f}),
      scale_(scale) {
  gUiComponents[sp_handle_.GetId() - details::kIdOffsetUi].parent_id_ =
      sp_track_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
  UpdateTransform();
}

UiSlider2D::UiSlider2D(UiSlider2D&& other) noexcept
    : UiBase(std::move(other)),
      sp_track_(std::move(other.sp_track_)),
      sp_handle_(std::move(other.sp_handle_)) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  scale_ = other.scale_;
  gUiComponents[sp_track_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

void UiSlider2D::SetParentTransform(LocalTransform transform) {
  sp_track_.SetParentTransform(transform);
  sp_handle_.SetParentTransform(transform);
}

void UiSlider2D::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    SetMousePos(mouse_pos);
  }
  sp_track_.Render();
  sp_handle_.Render();
}

void UiSlider2D::RenderPicking() const {
  sp_track_.RenderPicking();
  if (debug::gUiAltMode) {
    sp_handle_.RenderPicking();
  }
}

void UiSlider2D::Press() { pressed_ = true; }

void UiSlider2D::Release() { pressed_ = false; }

void UiSlider2D::SetValue(glm::vec2 value) { SetProgress(value / scale_); }

void UiSlider2D::SetMousePos(glm::vec2 mouse_pos) {
  glm::vec2 half_length_ = length_ / 2.0f;
  glm::vec2 offset =
      glm::clamp(mouse_pos - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  sp_handle_.SetTranslate(offset);
}

void UiSlider2D::SetProgress(glm::vec2 progress) {
  progress_ = glm::clamp(progress, 0.0f, 1.0f);
  glm::vec2 half_length_ = length_ / 2.0f;
  glm::vec2 offset = progress_ * length_ - half_length_;
  sp_handle_.SetTranslate(offset);
}

bool UiSlider2D::Scroll(GLuint id, float yoffset) {
  if (id != sp_track_.GetId() || id != sp_handle_.GetId()) {
    return false;
  }
  glm::vec2 progress = progress_;
  if (glfwGetKey(gWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    progress.x += 0.01f * yoffset;
  } else {
    progress.y += 0.01f * yoffset;
  }
  SetProgress(progress);
  return true;
}

glm::vec2 UiSlider2D::GetProgress() const { return progress_ * scale_; }

float UiSlider2D::GetProgressX() const { return progress_.x * scale_.x; }

float UiSlider2D::GetProgressY() const { return progress_.y * scale_.y; }

void UiSlider2D::UpdateTransform() {
  sp_track_.UpdateTransform();
  sp_handle_.UpdateTransform();
  length_ = {kTrackWidthFactor *
                 (sp_track_.GetRightBorder() - sp_track_.GetLeftBorder()),
             kTrackHeightFactor *
                 (sp_track_.GetTopBorder() - sp_track_.GetBottomBorder())};
  centre_ = {(sp_track_.GetRightBorder() + sp_track_.GetLeftBorder()) / 2.0f,
             (sp_track_.GetTopBorder() + sp_track_.GetBottomBorder()) / 2.0f};
  glm::vec2 related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  SetMousePos(related_pos);
}

UiToggle4::UiToggle4(UiDynamicSprite&& off, UiDynamicSprite&& on1,
                     UiDynamicSprite&& on2, UiDynamicSprite&& on3)
    : UiBase(off.GetId(), {}),
      off_(std::move(off)),
      on1_(std::move(on1)),
      on2_(std::move(on2)),
      on3_(std::move(on3)),
      state_(&off_),
      speed_(0.5f) {
  gUiComponents[on1_.GetId() - details::kIdOffsetUi].parent_id_ = off_.GetId();
  gUiComponents[on2_.GetId() - details::kIdOffsetUi].parent_id_ = off_.GetId();
  gUiComponents[on3_.GetId() - details::kIdOffsetUi].parent_id_ = off_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
  UpdateTransform();
}

UiToggle4::UiToggle4(UiToggle4&& other) noexcept
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
  gUiComponents[off_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

void UiToggle4::Render() {
  UpdateState();
  state_->Render();
}

void UiToggle4::Render(bool state) {
  turned_off_ = state;
  Render();
}

void UiToggle4::RenderPicking() const { off_.RenderPicking(); }

void UiToggle4::Press() {
  progress_ = 0.0f;
  turned_off_ = !turned_off_;
  off_.Press();
}

void UiToggle4::UpdateTransform() {
  off_.UpdateTransform();
  on1_.UpdateTransform();
  on2_.UpdateTransform();
  on3_.UpdateTransform();
}

void UiToggle4::SetParentTransform(LocalTransform transform) {
  off_.SetParentTransform(transform);
  on1_.SetParentTransform(transform);
  on2_.SetParentTransform(transform);
  on3_.SetParentTransform(transform);
}

void UiToggle4::SetTranslate(glm::vec2 translate) {
  off_.SetTranslate(translate);
  on1_.SetTranslate(translate);
  on2_.SetTranslate(translate);
  on3_.SetTranslate(translate);
}

void UiToggle4::Set(bool value) {
  if (turned_off_ != value) {
    Press();
  }
}

void UiToggle4::UpdateState() {
  progress_ += speed_ * gDeltaTime;
  progress_ = progress_ - static_cast<float>(static_cast<int>(progress_));
  if (turned_off_) {
    state_ = &off_;
  } else {
    if (progress_ < 0.33f) {
      state_ = &on1_;
    } else if (progress_ < 0.67f) {
      state_ = &on2_;
    } else {
      state_ = &on3_;
    }
  }
}

UiToggle2::UiToggle2(UiDynamicSprite&& off, UiDynamicSprite&& on)
    : UiBase(off.GetId(), {}), off_(std::move(off)), on_(std::move(on)) {
  gUiComponents[on_.GetId() - details::kIdOffsetUi].parent_id_ = off_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui = static_cast<UiBase*>(this);
  UpdateTransform();
}

UiToggle2::UiToggle2(UiToggle2&& other) noexcept
    : UiBase(std::move(other)),
      off_(std::move(other.off_)),
      on_(std::move(other.on_)),
      turned_off_(other.turned_off_) {
  gUiComponents[off_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

void UiToggle2::Render() {
  if (turned_off_) {
    off_.Render();
  } else {
    on_.Render();
  }
}

void UiToggle2::RenderPicking() const { off_.RenderPicking(); }

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

void UiToggle2::Set(bool value) {
  if (turned_off_ != value) {
    Press();
  }
}
