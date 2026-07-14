#ifndef WIREBOUNDWORLDCREATOR_POINTLIGHT_H
#define WIREBOUNDWORLDCREATOR_POINTLIGHT_H

#include <glm/glm.hpp>
#include "Scene.h"

struct PointLight {
  const Scene::ModelNode* object_;
  glm::vec3 color_ = glm::vec3(1.0f);
  float factor_linear_ = 1.0f;
  float factor_quadratic_ = 1.0f;
  float far_plane_ = 5.0f;
  float radius_ = 6.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_POINTLIGHT_H
