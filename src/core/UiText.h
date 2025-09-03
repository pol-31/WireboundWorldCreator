#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UITEXT_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UITEXT_H_

#include <glm/glm.hpp>

#include "Ui.h"
#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"

class UiTextInput final : public UiBase {
 public:
  UiTextInput(
      TextRenderer& text_renderer,
      UiDynamicSprite&& back,
      UiDynamicSprite&& text);

  UiTextInput(UiTextInput&& other) noexcept;

  UiTextInput(const UiTextInput& other) = delete;

  UiTextInput& operator=(UiTextInput&& other) = delete;
  UiTextInput& operator=(const UiTextInput& other) = delete;

  void Render();

  void RenderTextNoScissors();

  void RenderBack();

  void RenderPicking();

  void Press() override;

  void UpdateTransform() override;

  void SetText(std::string_view text);

  void SetText(const FixedSizeQueue<char, 64>& text);

  [[nodiscard]] float GetLeftBorder() const noexcept;

  [[nodiscard]] float GetRightBorder() const noexcept;

  void SetScissorArea();

  void SetTextTranslate(glm::vec2 translate) {
    translate_ = translate;
  }

  [[nodiscard]] glm::vec2 GetTextTranslate() const noexcept {
    return translate_;
  }

  /*void UpdateTransform() override;

  void SetParentTransform(LocalTransform transform);

  void SetTranslate(glm::vec2 translate);

  void SetTextScale(float scale) {
    scale_ = scale;
  }

  void SetTextTranslate(glm::vec2 translate) {
    translate_ = translate;
  }

  [[nodiscard]] float GetTextScale() const noexcept {
    return scale_;
  }

  [[nodiscard]] glm::vec2 GetTextTranslate() const noexcept {
    return translate_;
  }*/

 private:
  friend class TextRenderer;
  UiDynamicSprite back_; // print text (can be static, we don't set transform)
//  UiDynamicSprite text_; // clickable area (dynamic - we set transform at render)
  UiDynamicSprite text_;

  FixedSizeQueue<char, 64> text_input_;
  TextRenderer& text_renderer_;

  glm::vec2 translate_;
};

class UiTextLabelBase : public UiBase {
 public:
  UiTextLabelBase(
      TextRenderer& text_renderer,
      float scale,
      UiDynamicSprite&& text);

  UiTextLabelBase(UiTextLabelBase&& other) noexcept;

  UiTextLabelBase(const UiTextLabelBase& other) = delete;

  UiTextLabelBase& operator=(UiTextLabelBase&& other) = delete;
  UiTextLabelBase& operator=(const UiTextLabelBase& other) = delete;

  // implemented by Derived
//  void Render();

  // implemented by Derived
//  void RenderPicking();

  void Press() override;

  void UpdateTransform() override;

  void SetParentTransform(LocalTransform transform);

  void SetTranslate(glm::vec2 translate);
/*
  void SetScale(float scale) {
    scale_ = scale / ui_scale;
  }

  [[nodiscard]] float GetTranslate() const noexcept {
    return ui_translate;
  }

  [[nodiscard]] float GetScale() const noexcept {
    return scale_ * ui_scale;
  }*/

 protected:
  UiDynamicSprite text_;
  TextRenderer& text_renderer_;
  float scale_ = 1.0f;
};

/// all static labels should be drawn using this class;
class UiTextLabelId final : public UiTextLabelBase {
 public:
  UiTextLabelId(
      TextRenderer& text_renderer,
      float scale,
      UiDynamicSprite&& text,
      data::TextId text_id);

  UiTextLabelId(UiTextLabelId&& other) noexcept;

  UiTextLabelId(const UiTextLabelId& other) = delete;

  UiTextLabelId& operator=(UiTextLabelId&& other) = delete;
  UiTextLabelId& operator=(const UiTextLabelId& other) = delete;

  void Render();

  void RenderPicking();

  void SetText(data::TextId text_id) {
    text_id_ = text_id;
  }

 private:
  data::TextId text_id_;
};

/// UiSlots
class UiTextLabel final : public UiTextLabelBase {
 public:
  /// no text arg; text in the class supposed to change from time to time
  UiTextLabel(
      TextRenderer& text_renderer,
      float scale,
      UiDynamicSprite&& text);

  UiTextLabel(UiTextLabel&& other) noexcept;

  UiTextLabel(const UiTextLabel& other) = delete;

  UiTextLabel& operator=(UiTextLabel&& other) = delete;
  UiTextLabel& operator=(const UiTextLabel& other) = delete;

  void Render();

  void RenderPicking();

  void SetText(std::string_view label) {
    label_ = label;
  }

 private:
  std::string label_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UITEXT_H_
