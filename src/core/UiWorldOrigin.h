#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UIWORLDORIGIN_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UIWORLDORIGIN_H_

#include <glm/glm.hpp>

#include "../ui/UiRenderData.h"
#include "Ui.h"

class UiWorldOrigin {
 public:
  UiWorldOrigin(UiRenderData& render_data);

  void Render(glm::vec4 position, glm::vec4 color);

 private:
  UiRenderData& render_data_;
  UiSprite sp_origin_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UIWORLDORIGIN_H_
