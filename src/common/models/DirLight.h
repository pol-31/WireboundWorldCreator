#ifndef WIREBOUNDWORLDCREATOR_DIRLIGHT_H
#define WIREBOUNDWORLDCREATOR_DIRLIGHT_H

#include <glm/glm.hpp>

struct DirLight {
  glm::mat4 lightSpaceMatrix_;
  glm::vec3 dir_;
  glm::vec3 color_;
  float radius_;
  bool is_on_ = true;
};

#endif  // WIREBOUNDWORLDCREATOR_DIRLIGHT_H
