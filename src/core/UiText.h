#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UITEXT_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UITEXT_H_

#include <glm/glm.hpp>

#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"
#include "Ui.h"

class UiTextInput final : public UiBase {
 public:
  UiTextInput(TextRenderer& text_renderer, UiSprite&& back, UiSprite&& text,
              float extra_width = 0.0f);

  UiTextInput(UiTextInput&& other) noexcept;
  UiTextInput(const UiTextInput& other) = delete;

  UiTextInput& operator=(UiTextInput&& other) = delete;
  UiTextInput& operator=(const UiTextInput& other) = delete;

  void Render();

  void RenderTextNoScissors();

  void RenderText();

  void RenderBack();

  void RenderPicking();

  void Press() override;

  void UpdateTransform() override;

  void SetText(std::string_view text);

  [[nodiscard]] std::string_view GetText() const noexcept {
    return text_input_;
  }

  [[nodiscard]] float GetLeftBorder() const noexcept;

  [[nodiscard]] float GetRightBorder() const noexcept;

  void SetScissorArea();

  void SetTextTranslate(glm::vec2 translate) { translate_ = translate; }

  [[nodiscard]] glm::vec2 GetTextTranslate() const noexcept {
    return translate_;
  }

 private:
  friend class TextRenderer;
  UiSprite sp_back_;  // clickable area
  UiSprite sp_text_;  // print text

  std::string text_input_;
  TextRenderer& text_renderer_;

  glm::vec2 translate_ = glm::vec2(0.0f);

  /// extra_width_ extends/shrinks showing text area, can be negative
  /// (clickable area still belongs to back_)
  float extra_width_ = 0.0f;
};

class UiTextLabelBase : public UiBase {
 public:
  UiTextLabelBase(TextRenderer& text_renderer, UiSprite&& text);

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

 protected:
  UiSprite sp_text_;
  TextRenderer& text_renderer_;
};

/// render string (no prerender), used in UiSlots
class UiText final : public UiTextLabelBase {
 public:
  /// no text arg; text in the class supposed to change from time to time
  UiText(TextRenderer& text_renderer, UiSprite&& text);

  UiText(UiText&& other) noexcept = default;
  UiText(const UiText& other) = delete;

  UiText& operator=(UiText&& other) = delete;
  UiText& operator=(const UiText& other) = delete;

  void Render();

  void RenderPicking();

  void Render(
      std::string_view text, float scale, glm::vec2 translate,
      TextRenderer::Alignment alignment = TextRenderer::Alignment::kLeft);

  void RenderPicking(
      std::string_view text, float scale, glm::vec2 translate,
      TextRenderer::Alignment alignment = TextRenderer::Alignment::kLeft);

  void SetText(std::string_view label) { text_ = label; }

  [[nodiscard]] std::string_view GetText() const noexcept { return text_; }

 private:
  std::string text_;
};

/// prerendered menu text
class UiTextMenuId final : public UiTextLabelBase {
 public:
  UiTextMenuId(TextRenderer& text_renderer, UiSprite&& text,
               data::TextId text_id = data::TextId::kLoading);

  UiTextMenuId(UiTextMenuId&& other) noexcept = default;
  UiTextMenuId(const UiTextMenuId& other) = delete;

  UiTextMenuId& operator=(UiTextMenuId&& other) = delete;
  UiTextMenuId& operator=(const UiTextMenuId& other) = delete;

  void Render();

  void RenderPicking();

  void SetText(data::TextId text_id) { text_id_ = text_id; }

  [[nodiscard]] std::string_view GetText() const noexcept {
    return data::gText[static_cast<int>(text_id_)];
  }

 private:
  data::TextId text_id_;
};

/// prerendered menu text
class UiTextModeId final : public UiTextLabelBase {
 public:
  UiTextModeId(TextRenderer& text_renderer, UiSprite&& text,
               data::TextId text_id);

  UiTextModeId(UiTextModeId&& other) noexcept = default;
  UiTextModeId(const UiTextModeId& other) = delete;

  UiTextModeId& operator=(UiTextModeId&& other) = delete;
  UiTextModeId& operator=(const UiTextModeId& other) = delete;

  void Render();

  void RenderPicking();

  void SetText(data::TextId text_id) { text_id_ = text_id; }

 private:
  data::TextId text_id_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UITEXT_H_
