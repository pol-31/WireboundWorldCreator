#ifndef  WIREBOUNDWORLDCREATOR_DIRECTEDLIGHT_H
#define WIREBOUNDWORLDCREATOR_DIRECTEDLIGHT_H

#include <glm/glm.hpp>

#include "../../core/Frustum.h"

struct DirectedLight {
  glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
  glm::vec3 dir = glm::vec3(0.1f, -0.4f, -0.1f);
  glm::vec3 color = glm::vec3(1.0f);
  Frustum frustum;
};

#endif  //  WIREBOUNDWORLDCREATOR_DIRECTEDLIGHT_H
