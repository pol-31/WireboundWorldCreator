#ifndef WIREBOUNDWORLDCREATOR_UIRADIOBUTTON_H
#define WIREBOUNDWORLDCREATOR_UIRADIOBUTTON_H

#include <functional>

#include "UiElement.h"
#include "UiSprite.h"

class UiRadioButton final : public UiElement {
 public:
  using BaseType = UiElement;
  using BaseType::BaseType;
  using CallableType = std::function<void()>;

  UiRadioButton(data::UiId id, data::UiId& selected_id, UiSprite& sp_selected_,
                CallableType callback = {});

  void Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
              float rotate, glm::vec2 scale) override;

  void Press(unsigned int pressed_id) override;

  void MoveMouse(glm::vec2 mouse_pos, unsigned int hovered_id) override;

  static const glm::vec4 gColorDefault;
  static const glm::vec4 gColorHovered;

 private:
  data::UiId& selected_id_;
  UiSprite& sp_selected_;
  glm::vec4 color_ = gColorDefault;
  CallableType callback_;
};

// add UiRadioButton by UiElement::Add()
class UiRadioButtons final : public UiElement {
 public:
  using BaseType = UiElement;
  using BaseType::BaseType;

  UiRadioButtons(data::UiId sp_selected);

  data::UiId& GetSelectedIdRef() { return selected_id_; }

  UiSprite& GetSelectedSpriteRef() { return sp_selected_; }

  static const glm::vec4 gColorDefault;
  static const glm::vec4 gColorHovered;

 private:
  data::UiId selected_id_ = static_cast<data::UiId>(-1);
  glm::vec4 color_ = gColorDefault;
  UiSprite sp_selected_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIRADIOBUTTON_H
