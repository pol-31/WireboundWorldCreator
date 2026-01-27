#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UIWORLDORIGIN_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UIWORLDORIGIN_H_

#include <glm/glm.hpp>

#include "../modes/UiSharedResources.h"
#include "Ui.h"

class UiWorldOrigin {
 public:
  UiWorldOrigin(UiSharedResources& ui_shared_resources);

  void Render(glm::vec4 position, glm::vec4 color);

 private:
  UiSharedResources& ui_shared_resources_;
  UiSprite sp_origin_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UIWORLDORIGIN_H_
