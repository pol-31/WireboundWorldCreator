#ifndef WIREBOUNDWORLDCREATOR_UIBUTTON_H
#define WIREBOUNDWORLDCREATOR_UIBUTTON_H

#include <functional>

#include "UiElement.h"

class UiButton final : public UiElement {
 public:
  using BaseType = UiElement;
  using BaseType::BaseType;
  using CallableType = std::function<void()>;

  UiButton(data::UiId id, CallableType callback = {});

  void Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
              float rotate, glm::vec2 scale) override;

  void Press(unsigned int pressed_id) override;

  void MoveMouse(glm::vec2 mouse_pos, unsigned int hovered_id) override;

  static const glm::vec4 gColorDefault;
  static const glm::vec4 gColorHovered;

 private:
  glm::vec4 color_ = gColorDefault;
  CallableType callback_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIBUTTON_H
