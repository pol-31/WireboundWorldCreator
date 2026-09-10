#ifndef WIREBOUNDWORLDCREATOR_POINTLIGHT_H
#define WIREBOUNDWORLDCREATOR_POINTLIGHT_H

#include <glm/glm.hpp>

#include "../../core/Frustum.h"

class SceneNode;

struct PointLight {
  struct CullingData {
    Frustum frustum;
    bool is_visible = true;
  };

  const SceneNode* object_;
  glm::vec3 color_ = glm::vec3(1.0f);
  float factor_linear_ = 0.09f;
  float factor_quadratic_ = 0.032f;
  float radius_ = 5.0f;
  std::array<CullingData, 6> frustum_;
};

#endif  // WIREBOUNDWORLDCREATOR_POINTLIGHT_H
