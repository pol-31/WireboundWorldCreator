#ifndef WIREBOUNDWORLDCREATOR_DOOR_H
#define WIREBOUNDWORLDCREATOR_DOOR_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "GameObject.h"

class Scene;
class ScenePortal;

struct HingeData {
  JPH::BodyID hitbox_id;  // The small physics body of the hinge mesh (what the
                          // bullet hits)
  JPH::Constraint* constraint;  // The Jolt constraint keeping the door attached
                                // at this pivot
  bool is_broken = false;
};

class Door : public GameObject {
 public:
  Door(JPH::PhysicsSystem* physics_system, const Scene* scene,
       ScenePortal* portal);

  void TakeDamage(JPH::Vec3 impulse, JPH::RVec3 hit_pos, float damage) override;

  std::string GetInteractPrompt() override;

 private:
  JPH::PhysicsSystem* physics_system_ = nullptr;
  ScenePortal* portal_ = nullptr;
  std::vector<HingeData> hinges_;
};

#endif  // WIREBOUNDWORLDCREATOR_DOOR_H
