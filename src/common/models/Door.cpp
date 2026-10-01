#include "Door.h"

#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>

#include <iostream>

#include "Hinge.h"
#include "Scene.h"

void Door::Initialize(JPH::PhysicsSystem* physics_system,
  ScenePortal* portal, Hinge* hinge_top, Hinge* hinge_bottom) {
  hinge_top_ = hinge_top;
  hinge_bottom_ = hinge_bottom;
  physics_system_ = physics_system;
  portal_ = portal;
  JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
  auto InitHinge = [&](Hinge* hinge) {
    JPH::HingeConstraintSettings hinge_settings;
    JPH::Vec3 pivot = hinge->GetSceneNode()->global_transform.GetTranslation();
    hinge_settings.mPoint1 = hinge_settings.mPoint2 = pivot;
    hinge_settings.mHingeAxis1 = hinge_settings.mHingeAxis2 = JPH::Vec3::sAxisY(); // Assuming Y is up
    JPH::Ref<JPH::Constraint> constraint = bi.CreateConstraint(
        &hinge_settings,
        portal->frame->body_id,
        portal->desk->body_id
    );
    physics_system_->AddConstraint(constraint);
    hinge->SetConstraint(constraint);
  };
  InitHinge(hinge_top_);
  InitHinge(hinge_bottom_);
}

GameObject::InteractPrompt Door::GetInteractPrompt(Character* interactor) {
  if (hinge_top_->IsBroken() && hinge_bottom_->IsBroken()) {
    return {};
  }
  return {"E to open the door", GetPosition()};
}

void Door::Interact(Character* interactor) {
  // SlowlyOpenByCharacter();
}

JPH::Vec3 Door::GetPosition() const {
  return portal_->position;
}
