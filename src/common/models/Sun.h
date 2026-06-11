#ifndef WIREBOUNDWORLDCREATOR_SUN_H
#define WIREBOUNDWORLDCREATOR_SUN_H

#include <glm/glm.hpp>
#include "../../core/Frustum.h"

struct Sun {
  glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
  glm::vec3 dir = glm::vec3(0.1f, -0.4f, -0.1f);
  glm::vec3 color = glm::vec3(1.0f);
  bool is_on = true;
  Frustum frustum;
};

#endif  // WIREBOUNDWORLDCREATOR_SUN_H
