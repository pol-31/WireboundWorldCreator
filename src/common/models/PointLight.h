#ifndef WIREBOUNDWORLDCREATOR_POINTLIGHT_H
#define WIREBOUNDWORLDCREATOR_POINTLIGHT_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <glm/glm.hpp>

#include "Scene.h"

struct PointLight {
  const Scene::Model& model;
  JPH::BodyID body_id_;
  glm::vec3 color_ = glm::vec3(1.0f);
  float factor_linear_ = 1.0f;
  float factor_quadratic_ = 1.0f;
  float far_plane_ = 5.0f;
  float radius_ = 6.0f;
  bool is_on_ = true;

  PointLight(const Scene::Model& inModel, JPH::BodyID inBodyId)
  : model(inModel), body_id_(inBodyId) {}
};

#endif  // WIREBOUNDWORLDCREATOR_POINTLIGHT_H
