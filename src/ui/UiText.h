#ifndef WIREBOUNDWORLDCREATOR_UITEXT_H
#define WIREBOUNDWORLDCREATOR_UITEXT_H

#include <string_view>

#include "UiElement.h"

class UiText final : public UiElement {
 public:
  using BaseType = UiElement;
  using BaseType::BaseType;

  void Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
              float rotate, glm::vec2 scale) override;

  void SetText(std::string_view text);

 private:
  std::string_view text_;
  // unsigned int id_text_ = 0; // generated internally from prerenderer fbo tex
};

#endif  // WIREBOUNDWORLDCREATOR_UITEXT_H
