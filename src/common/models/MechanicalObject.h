#ifndef WIREBOUNDWORLDCREATOR_MECHANICALOBJECT_H
#define WIREBOUNDWORLDCREATOR_MECHANICALOBJECT_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>

#include "Scene.h"

class MechanicalObject {
public:
  MechanicalObject(Scene::ModelNode* model);

  void UpdateRenderTransform(const JPH::BodyLockInterface& bli);

  void DbgUpdateRenderTransform(const JPH::BodyLockInterface& bli);

  bool TakeDamage(JPH::BodyID hit_body, float damage);

private:
  /// render hierarchy, includes all meshes with their types (hinge or what)
  Scene::ModelNode* model;

  /// size == model.nodes.size(), id invalid if node hasn't the body
  std::vector<JPH::BodyID> physics_bodies_;

  /// bodies engaged in constraints, can be destroyed
  /// other bodies handled by JoltPhysics, we don't care about them
  std::vector<JPH::BodyID> hinges_;

  /// physics hierarchy
  std::vector<JPH::Constraint*> constraints_;

    // glm::vec3 color_{1.0f}; // global instance color
  };

#endif  // WIREBOUNDWORLDCREATOR_MECHANICALOBJECT_H
