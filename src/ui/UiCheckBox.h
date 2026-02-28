#ifndef WIREBOUNDWORLDCREATOR_UICHECKBOX_H
#define WIREBOUNDWORLDCREATOR_UICHECKBOX_H

#include "UiElement.h"

// TODO: sprite sheet here would be good

/// implemented as a UiSprite for state off
class UiCheckBox final : public UiElement {
 public:
  using BaseType = UiElement;

  UiCheckBox(data::UiId id_on, data::UiId id_off);

  void Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
              float rotate, glm::vec2 scale) override;

  void Press(unsigned int pressed_id) override;

  [[nodiscard]] bool GetValue() const noexcept { return value_; }

  void SetValue(bool value) { value_ = value; }

  static const glm::vec4 gColorDefault;
  static const glm::vec4 gColorHovered;

 private:
  glm::vec4 color_ = gColorDefault;

  data::UiId sprite_id_on_;

  bool value_ = false;
};

#endif  // WIREBOUNDWORLDCREATOR_UICHECKBOX_H
