#ifndef WIREBOUNDWORLDCREATOR_UIGAMEMAP_H
#define WIREBOUNDWORLDCREATOR_UIGAMEMAP_H

#include "../UiAnimation.h"
#include "../UiElement.h"

class UiGameMap final : public UiElement {
 public:
  UiGameMap();

  void Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
              float rotate, glm::vec2 scale) override;

  void MoveMouse(glm::vec2 mouse_pos, unsigned int hovered_id) override;

 private:
  UiScissorRect GetScissorsRect();

  UiAnimationTransform animation_;
  bool hovered_ = false;
};

#endif  // WIREBOUNDWORLDCREATOR_UIGAMEMAP_H
