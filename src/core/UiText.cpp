#include "UiText.h"

UiTextInput::UiTextInput(
    TextRenderer& text_renderer,
    UiDynamicSprite&& back,
    UiDynamicSprite&& text)
    : UiBase(back.GetId(), {}),
      text_renderer_(text_renderer),
      back_(std::move(back)),
      text_(std::move(text)),
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
      text_input_(std::move(other.text_input_)) {
  gUiComponents[back_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiTextInput::Render() {
  back_.Render();
  glEnable(GL_SCISSOR_TEST);
  SetScissorArea();
  text_renderer_.RenderText(text_, &text_input_, translate_);
  glDisable(GL_SCISSOR_TEST);
}

void UiTextInput::RenderTextNoScissors() {
  text_renderer_.RenderText(text_, &text_input_, translate_);
}

void UiTextInput::RenderBack() {
  back_.Render();
}

void UiTextInput::RenderPicking() {
  back_.RenderPicking();
  if (debug::gUiAltMode) {
    glEnable(GL_SCISSOR_TEST);
    SetScissorArea();
    text_renderer_.RenderPickingText(text_, &text_input_);
    glDisable(GL_SCISSOR_TEST);
  }
}

void UiTextInput::Press() {
  text_renderer_.BindCallbacks();
//  text_renderer_.StartInput(text_, &text_input_);
}

void UiTextInput::UpdateTransform() {
  back_.UpdateTransform();
  text_.UpdateTransform();
}

void UiTextInput::SetText(std::string_view text) {
  text_input_.Clear();
  for (auto c : text) {
    text_input_.PushBack(c);
  }
}
void UiTextInput::SetText(const FixedSizeQueue<char, 64>& text) {
  text_input_ = text;
}

float UiTextInput::GetLeftBorder() const noexcept {
  return back_.GetLeftBorder();
}

float UiTextInput::GetRightBorder() const noexcept {
  return back_.GetRightBorder();
}

void UiTextInput::SetScissorArea() {
  // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
  auto x_start =
      int((back_.GetLeftBorder() + 1) * 0.5f * gWindowWidth);
  auto x_length =
      int((back_.GetRightBorder()
           - back_.GetLeftBorder()) * 0.5f * gWindowWidth);
  glScissor(x_start, 0, x_length, 4000);
}

// як я до цього прийшов: хочу скейл для тексту, але текст-компонентів багато.
// або скейлить всі поодинці або додать скейл параметер АБО автоматизувать з UiBlock
// що також вирішить проблему вирівнювання
//class UiBlock {
// public:
//  UiBlock(UiDynamicSprite&& daddy, float scale)
//      : daddy_(std::move(daddy)) {
//    //TODO: set scale to daddy_
//  }
//
//  void Attach(UiBase* component, glm::vec2 offset, float scale) {
//    component->parent = id_;
//    component->ui = this;
//    component->transform = {offset, scale};
//    components_.push_back(component);
//  }
//
//  void Render() {
//    if (debug::gUiAltMode) {
//      daddy_.Render();
//    }
//    for (auto component : components_) {
//      component->Render(); // TODO: some requires mouse pos
//    }
//  }
//
// private:
//  UiDynamicSprite daddy_; // all other depends on this
//  std::vector<UiBase*> components_;
//};

UiTextLabelBase::UiTextLabelBase(
    TextRenderer& text_renderer,
    float scale,
    UiDynamicSprite&& text)
    : UiBase(text.GetId(), {}),
      text_renderer_(text_renderer),
      text_(std::move(text)),
      scale_(scale) {
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UpdateTransform();
}

UiTextLabelBase::UiTextLabelBase(UiTextLabelBase&& other) noexcept
    : UiBase(std::move(other)),
      text_renderer_(other.text_renderer_),
      text_(std::move(other.text_)),
      scale_(other.scale_) {
  gUiComponents[text_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiTextLabelBase::Press() {}

void UiTextLabelBase::UpdateTransform() {
  text_.UpdateTransform();
}

void UiTextLabelBase::SetParentTransform(LocalTransform transform) {
  text_.SetParentTransform(transform);
}

void UiTextLabelBase::SetTranslate(glm::vec2 translate) {
  text_.SetTranslate(translate);
}

UiTextLabelId::UiTextLabelId(
    TextRenderer& text_renderer,
    float scale,
    UiDynamicSprite&& text,
    data::TextId text_id)
    : UiTextLabelBase(text_renderer, scale, std::move(text)),
      text_id_(text_id) {
  UpdateTransform();
}

UiTextLabelId::UiTextLabelId(UiTextLabelId&& other) noexcept
    : UiTextLabelBase(std::move(other)),
      text_id_(other.text_id_) {}

void UiTextLabelId::Render() {
  text_renderer_.RenderMenuText(text_, text_id_, scale_);
}

void UiTextLabelId::RenderPicking() {
  text_renderer_.RenderMenuTextPicking(text_, text_id_);
}

UiTextLabel::UiTextLabel(
    TextRenderer& text_renderer,
    float scale,
    UiDynamicSprite&& text)
    : UiTextLabelBase(text_renderer, scale, std::move(text)),
      label_() {
  UpdateTransform();
}

UiTextLabel::UiTextLabel(UiTextLabel&& other) noexcept
    : UiTextLabelBase(std::move(other)),
      label_(std::move(other.label_)) {}

void UiTextLabel::Render() {
  text_renderer_.RenderText(text_, label_, scale_, glm::vec2{0.0f});
}

void UiTextLabel::RenderPicking() {
  text_renderer_.RenderTextPicking(text_, label_, scale_, glm::vec2{0.0f});
}
