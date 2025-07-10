#include "UiText.h"

UiTextInput::UiTextInput(
    TextRenderer& text_renderer,
    float scale,
    glm::vec2 translate,
    UiDynamicSprite&& back,
    UiDynamicSprite&& text)
    : UiBase(back.GetId(), {}),
      text_renderer_(text_renderer),
      back_(std::move(back)),
      text_(std::move(text)),
      scale_(scale),
      translate_(translate),
      text_input_() {
  gUiComponents[text_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiTextInput::UiTextInput(UiTextInput&& other) noexcept
    : UiBase(std::move(other)),
      text_renderer_(other.text_renderer_),
      back_(std::move(other.back_)),
      text_(std::move(other.text_)),
      scale_(other.scale_),
      translate_(other.translate_),
      text_input_(std::move(other.text_input_)) {
  gUiComponents[back_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiTextInput::Render() {
  back_.Render();
  text_renderer_.RenderText(text_, &text_input_, scale_, translate_);
}

void UiTextInput::RenderPicking() {
  back_.RenderPicking();
  if (debug::gCtrlMode) {
    text_renderer_.RenderTextPicking(text_, &text_input_, scale_, translate_);
  }
}

void UiTextInput::Press() {
  text_renderer_.BindCallbacks();
  text_renderer_.StartInput(text_, &text_input_);
}

data::TextId UiTextInput::Hover(std::uint32_t id) {
  return back_.Hover();
}

void UiTextInput::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  back_.UpdateTransform();
  text_.UpdateTransform();
}

void UiTextInput::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

void UiTextInput::SetParentTransform(LocalTransform transform) {
  back_.SetParentTransform(transform);
  text_.SetParentTransform(transform);
}

void UiTextInput::SetTranslate(glm::vec2 translate) {
  back_.SetTranslate(translate);
  text_.SetTranslate(translate);
}

UiTextLabel::UiTextLabel(
    TextRenderer& text_renderer,
    float scale,
    glm::vec2 translate,
    UiDynamicSprite&& text)
    : UiBase(text.GetId(), {}),
      text_renderer_(text_renderer),
      text_(std::move(text)),
      scale_(scale),
      translate_(translate),
      label_() {
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiTextLabel::UiTextLabel(UiTextLabel&& other) noexcept
    : UiBase(std::move(other)),
      text_renderer_(other.text_renderer_),
      text_(std::move(other.text_)),
      scale_(other.scale_),
      translate_(other.translate_),
      label_(std::move(other.label_)) {
  gUiComponents[text_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiTextLabel::Render() {
  text_renderer_.RenderText(text_, label_, scale_, translate_);
}

void UiTextLabel::RenderPicking() {
  text_renderer_.RenderTextPicking(text_, label_, scale_, translate_);
}

void UiTextLabel::Press() {}

data::TextId UiTextLabel::Hover(std::uint32_t id) {
  return text_.Hover();
}

void UiTextLabel::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  text_.UpdateTransform();
}

void UiTextLabel::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

void UiTextLabel::SetParentTransform(LocalTransform transform) {
  text_.SetParentTransform(transform);
}

void UiTextLabel::SetTranslate(glm::vec2 translate) {
  text_.SetTranslate(translate);
}
