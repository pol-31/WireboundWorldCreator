#include "Door.h"

#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>

#include <iostream>

#include "Hinge.h"
#include "Scene.h"

void Door::Initialize(JPH::PhysicsSystem* physics_system,
  SceneNode* scene_node, JPH::BodyID frame_body_id) {
  if (physics_system_) {
    throw std::runtime_error("the door already initialized");
  }
  physics_system_ = physics_system;
  scene_node_ = scene_node;
  JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
  auto InitHinge = [&](Hinge* hinge) {
    JPH::HingeConstraintSettings hinge_settings;
    JPH::Vec3 pivot = hinge->GetSceneNode()->global_transform.GetTranslation();
    hinge_settings.mPoint1 = hinge_settings.mPoint2 = pivot;
    hinge_settings.mHingeAxis1 = hinge_settings.mHingeAxis2 = JPH::Vec3::sAxisY();
    JPH::Ref<JPH::Constraint> constraint = bi.CreateConstraint(
        &hinge_settings,
        frame_body_id,
        scene_node_->body_id
    );
    physics_system_->AddConstraint(constraint);
    hinge->SetConstraint(constraint);
  };
  for (Hinge* h : hinges_) {
    InitHinge(h);
  }
}

GameObject::InteractPrompt Door::GetInteractPrompt(Character* interactor) {
  bool all_broken = true;
  for (Hinge* h : hinges_) {
    if (!h->IsBroken()) {
      all_broken = false;
      break;
    }
  }
  if (all_broken) {
    return {};
  }
  return {"E to open the door", GetPosition()};
}

void Door::Interact(Character* interactor) {
  // SlowlyOpenByCharacter();
}

JPH::Vec3 Door::GetPosition() const {
  return scene_node_->global_bounds.GetCenter();
}
