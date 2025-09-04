#include "UiText.h"

UiTextInput::UiTextInput(
    TextRenderer& text_renderer,
    UiDynamicSprite&& back,
    UiDynamicSprite&& text,
    float extra_width)
    : UiBase(back.GetId(), {}),
      text_renderer_(text_renderer),
      sp_back_(std::move(back)),
      sp_text_(std::move(text)),
      text_input_(),
      extra_width_(extra_width + 0.1f) {
  gUiComponents[sp_text_.GetId() - details::kIdOffsetUi].parent_id_
      =
      sp_back_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiTextInput::UiTextInput(UiTextInput&& other) noexcept
    : UiBase(std::move(other)),
      text_renderer_(other.text_renderer_),
      sp_back_(std::move(other.sp_back_)),
      sp_text_(std::move(other.sp_text_)),
      text_input_(std::move(other.text_input_)),
      extra_width_(other.extra_width_){
  gUiComponents[sp_back_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiTextInput::Render() {
  sp_back_.Render();
  glEnable(GL_SCISSOR_TEST);
  SetScissorArea();
  text_renderer_.RenderText(
      sp_text_, text_input_, 1.0f,
      glm::vec2{translate_.x - extra_width_, translate_.y},
      TextRenderer::Alignment::kLeft);
  glDisable(GL_SCISSOR_TEST);
}

void UiTextInput::RenderTextNoScissors() {
  text_renderer_.RenderText(
      sp_text_, text_input_, 1.0f,
      glm::vec2{translate_.x - extra_width_, translate_.y},
      TextRenderer::Alignment::kLeft);
}

void UiTextInput::RenderText() {
  glEnable(GL_SCISSOR_TEST);
  SetScissorArea();
  text_renderer_.RenderText(
      sp_text_, text_input_, 1.0f,
      glm::vec2{translate_.x - extra_width_, translate_.y},
      TextRenderer::Alignment::kLeft);
  glDisable(GL_SCISSOR_TEST);
}

void UiTextInput::RenderBack() {
  sp_back_.Render();
}

void UiTextInput::RenderPicking() {
  sp_back_.RenderPicking();
  if (debug::gUiAltMode) {
    glEnable(GL_SCISSOR_TEST);
    SetScissorArea();
    text_renderer_.RenderTextPicking(
        sp_text_, text_input_, 1.0f,
        glm::vec2{translate_.x - extra_width_, translate_.y},
        TextRenderer::Alignment::kLeft);
    glDisable(GL_SCISSOR_TEST);
  }
}

void UiTextInput::Press() {
  text_renderer_.StartInput(this);
}

void UiTextInput::UpdateTransform() {
  sp_back_.UpdateTransform();
  sp_text_.UpdateTransform();
}

void UiTextInput::SetText(std::string_view text) {
  text_input_ = text;
}

float UiTextInput::GetLeftBorder() const noexcept {
  return sp_back_.GetLeftBorder() - extra_width_;
}

float UiTextInput::GetRightBorder() const noexcept {
  return sp_back_.GetRightBorder() + extra_width_;
}

void UiTextInput::SetScissorArea() {
  // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
  auto x_start =
      int((GetLeftBorder() + 1) * 0.5f * gWindowWidth);
  auto x_length =
      int((GetRightBorder() - GetLeftBorder()) * 0.5f * gWindowWidth);
  glScissor(x_start, 0, x_length, 4000);
}

UiTextLabelBase::UiTextLabelBase(
    TextRenderer& text_renderer,
    UiDynamicSprite&& text)
    : UiBase(text.GetId(), {}),
      text_renderer_(text_renderer),
      sp_text_(std::move(text)) {
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiTextLabelBase::UiTextLabelBase(UiTextLabelBase&& other) noexcept
    : UiBase(std::move(other)),
      text_renderer_(other.text_renderer_),
      sp_text_(std::move(other.sp_text_)) {
  gUiComponents[sp_text_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiTextLabelBase::Press() {}

void UiTextLabelBase::UpdateTransform() {
  sp_text_.UpdateTransform();
}

void UiTextLabelBase::SetParentTransform(LocalTransform transform) {
  sp_text_.SetParentTransform(transform);
}

void UiTextLabelBase::SetTranslate(glm::vec2 translate) {
  sp_text_.SetTranslate(translate);
}

UiText::UiText(
    TextRenderer& text_renderer,
    UiDynamicSprite&& text)
    : UiTextLabelBase(text_renderer, std::move(text)),
      text_() {
  UpdateTransform();
}

void UiText::Render() {
  text_renderer_.RenderText(sp_text_, text_, 1.0f, glm::vec2{0.0f});
}

void UiText::Render(
    std::string_view text, float scale, glm::vec2 translate) {
  text_renderer_.RenderText(sp_text_, text, scale, translate);
}

void UiText::RenderPicking(
    std::string_view text, float scale, glm::vec2 translate) {
  text_renderer_.RenderTextPicking(sp_text_, text, scale, translate);
}

void UiText::RenderPicking() {
  text_renderer_.RenderTextPicking(sp_text_, text_, 1.0f, glm::vec2{0.0f});
}

UiTextMenuId::UiTextMenuId(
    TextRenderer& text_renderer,
    UiDynamicSprite&& text,
    data::TextId text_id)
    : UiTextLabelBase(text_renderer, std::move(text)),
      text_id_(text_id) {
  UpdateTransform();
}

void UiTextMenuId::Render() {
  text_renderer_.RenderMenuText(sp_text_, text_id_);
}

void UiTextMenuId::RenderPicking() {
  text_renderer_.RenderMenuTextPicking(sp_text_);
}

UiTextModeId::UiTextModeId(
    TextRenderer& text_renderer,
    UiDynamicSprite&& text,
    data::TextId text_id)
    : UiTextLabelBase(text_renderer, std::move(text)),
      text_id_(text_id) {
  UpdateTransform();
}

void UiTextModeId::Render() {
  text_renderer_.RenderModeText(sp_text_, text_id_);
}

void UiTextModeId::RenderPicking() {
  text_renderer_.RenderModeTextPicking(sp_text_);
}
