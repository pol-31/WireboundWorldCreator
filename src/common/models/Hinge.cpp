#include "Hinge.h"

#include <iostream>

#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include "Scene.h"

Hinge::Hinge(JPH::PhysicsSystem* physics_system,
      const SceneNode* scene_node)
    : physics_system_(physics_system),
      scene_node_(scene_node) {}

void Hinge::TakeDamage(JPH::Vec3 impulse, JPH::RVec3 hit_pos, float damage) {
  if (Dismantle()) {
    JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
    bi.AddImpulse(scene_node_->body_id, impulse);
  }
}

GameObject::InteractPrompt Hinge::GetInteractPrompt(Character* interactor) {
  if (is_broken_) {
    return {};
  }
  return {"E to dismantle", GetPosition()};
}

bool Hinge::Dismantle() {
  if (is_broken_) {
    return false;
  }
  physics_system_->RemoveConstraint(constraint_);
  JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
  bi.SetMotionType(
      scene_node_->body_id,             // The BodyID of the hinge you shot
      JPH::EMotionType::Dynamic,   // Switch it to dynamic
      JPH::EActivation::Activate   // Wake it up immediately so gravity takes effect
  );
  is_broken_ = true;
  return true;
}

void Hinge::Interact(Character* interactor) {
  Dismantle();
}

JPH::Vec3 Hinge::GetPosition() const {
  return scene_node_->global_bounds.GetCenter();
}

