#ifndef WIREBOUNDWORLDCREATOR_UICOMPASS_H
#define WIREBOUNDWORLDCREATOR_UICOMPASS_H

#include "../UiElement.h"
#include "../UiSprite.h"

class Camera;

// UiElement as a compass itself
class UiCompass final : public UiElement {
 public:
  UiCompass(const Camera* camera);

  void Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
              float rotate, glm::vec2 scale) override;

 private:
  const Camera* camera_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_UICOMPASS_H
