#ifndef WIREBOUNDWORLDCREATOR_DOOR_H
#define WIREBOUNDWORLDCREATOR_DOOR_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "GameObject.h"

class Scene;
class ScenePortal;
class Hinge;

class Door : public GameObject {
 public:
  Door() = default;

  void Initialize(JPH::PhysicsSystem* physics_system,
       ScenePortal* portal, Hinge* hinge_top, Hinge* hinge_bottom);

  GameObject::InteractPrompt GetInteractPrompt(Character* interactor) override;

  void Interact(Character* interactor) override;

  JPH::Vec3 GetPosition() const override;

 private:
  JPH::PhysicsSystem* physics_system_ = nullptr;
  ScenePortal* portal_ = nullptr;
  Hinge* hinge_top_ = nullptr;
  Hinge* hinge_bottom_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_DOOR_H
