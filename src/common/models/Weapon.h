#ifndef WIREBOUNDWORLDCREATOR_WEAPON_H
#define WIREBOUNDWORLDCREATOR_WEAPON_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "Scene.h"
#include "../../core/Frustum.h"

class Weapon {
public:
  enum class OperationalState { Idle, Reload, Shoot };
  enum class PhysicsState { Carried, Dropped };

  Weapon(const Scene::WeaponData* model, JPH::BodyInterface& bi)
        : model_(model), body_interface_(bi) {}

  void Shoot(const JPH::Vec3& eye_pos, const JPH::Vec3& forward_dir);
  void Reload();
  void Update(float dt);

  void Equip(class Character* new_owner);
  void Drop(const JPH::Vec3& position, const JPH::Vec3& impulse);

  const Scene::WeaponData* GetModel() const { return model_; }

  void Render(const Frustum& frustum);

private:
  JPH::BodyInterface& body_interface_;
  const Scene::WeaponData* model_;

  OperationalState op_state_ = OperationalState::Idle;
  PhysicsState phys_state_ = PhysicsState::Dropped;

  int bullets_left_ = 0;

  // Only valid if PhysicsState::Dropped
  Character* owner_ = nullptr;
  JPH::BodyID jph_body_id_;
};

#endif  // WIREBOUNDWORLDCREATOR_WEAPON_H
