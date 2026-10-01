#ifndef WIREBOUNDWORLDCREATOR_HINGE_H
#define WIREBOUNDWORLDCREATOR_HINGE_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "GameObject.h"

class SceneNode;

class Hinge : public GameObject {
public:
  Hinge(JPH::PhysicsSystem* physics_system, const SceneNode* scene_node);

  void Interact(Character* interactor) override;

  void TakeDamage(JPH::Vec3 impulse, JPH::RVec3 hit_pos, float damage) override;

  GameObject::InteractPrompt GetInteractPrompt(Character* interactor) override;

  JPH::Vec3 GetPosition() const override;

  void SetConstraint(JPH::Ref<JPH::Constraint> constraint) {
    constraint_ = constraint;
  }

  [[nodiscard]] bool IsBroken() const noexcept {
    return is_broken_;
  }

  [[nodiscard]] const SceneNode* GetSceneNode() const noexcept {
    return scene_node_;
  }

private:
  bool Dismantle();

  JPH::PhysicsSystem* physics_system_ = nullptr;
  JPH::Ref<JPH::Constraint> constraint_ = nullptr;
  bool is_broken_ = false;
  const SceneNode* scene_node_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_HINGE_H
