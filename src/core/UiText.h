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
      float scale,
      glm::vec2 translate,
      UiDynamicSprite&& back,
      UiDynamicSprite&& text);

  UiTextInput(UiTextInput&& other) noexcept;

  UiTextInput(const UiTextInput& other) = delete;

  UiTextInput& operator=(UiTextInput&& other) = delete;
  UiTextInput& operator=(const UiTextInput& other) = delete;

  void Render();

  void RenderPicking();

  void Press() override;

  data::TextId Hover(std::uint32_t id);

  void UpdateTransform(
      float x_translate, float y_translate, float scale) override;

  void UpdateTransform() override;

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
  }

 private:
  UiDynamicSprite back_; // clickable area
  UiDynamicSprite text_; // print text

  TextRenderer& text_renderer_;
  FixedSizeQueue<char, 64> text_input_;
  float scale_ = 1.0f;
  glm::vec2 translate_ = glm::vec2{0.0f};
};

class UiTextLabel final : public UiBase {
 public:
  UiTextLabel(
      TextRenderer& text_renderer,
      float scale,
      glm::vec2 translate,
      UiDynamicSprite&& text);

  UiTextLabel(UiTextLabel&& other) noexcept;

  UiTextLabel(const UiTextLabel& other) = delete;

  UiTextLabel& operator=(UiTextLabel&& other) = delete;
  UiTextLabel& operator=(const UiTextLabel& other) = delete;

  void Render();

  void RenderPicking();

  void Press() override;

  data::TextId Hover(std::uint32_t id);

  void UpdateTransform(
      float x_translate, float y_translate, float scale) override;

  void UpdateTransform() override;

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
  }

  void SetText(std::string_view label) {
    label_ = label;
  }

 private:
  UiDynamicSprite text_;

  TextRenderer& text_renderer_;
  std::string label_;
  float scale_ = 1.0f;
  glm::vec2 translate_ = glm::vec2{0.0f};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UITEXT_H_
