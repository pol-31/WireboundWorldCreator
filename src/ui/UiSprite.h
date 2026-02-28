#ifndef WIREBOUNDWORLDCREATOR_UISPRITE_H
#define WIREBOUNDWORLDCREATOR_UISPRITE_H

#include
#include
#include "UiElement.h"

class UiSprite final : public UiElement {
 public:
  using BaseType = UiElement;
  using BaseType::BaseType;

  void Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
              float rotate, glm::vec2 scale) override;
};

#endif  // WIREBOUNDWORLDCREATOR_UISPRITE_H
