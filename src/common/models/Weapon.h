#ifndef WIREBOUNDWORLDCREATOR_WEAPON_H
#define WIREBOUNDWORLDCREATOR_WEAPON_H

#include "Scene.h"

namespace JPH {
class PhysicsSystem;
} // namespace JPH

class Animator;

class Weapon {
public:
  enum class OperationalState { Idle, Reload, Shoot };

  Weapon(const Scene::WeaponData* model,
    JPH::PhysicsSystem* physics_system,
    Animator* animator);

  /// --- all interaction interface ---
  bool TryShoot(const JPH::Vec3& eye_pos, const JPH::Vec3& forward_dir);
  void Reload();
  void Update(float dt);

  void Equip(class Character* new_owner);
  void Drop(const JPH::Vec3& position, const JPH::Vec3& impulse);

  const Scene::WeaponData* GetModel() const { return model_; }

  AnimatedRenderData GetAnimatedRenderData() const;

  [[nodiscard]] JPH::Vec3 GetPosition() const;

  [[nodiscard]] bool HasOwner() const noexcept {
    return static_cast<bool>(owner_);
  }

private:
  void CreatePhysicBody(const JPH::Vec3& position);

  void DeletePhysicBody();

  JPH::PhysicsSystem* physics_system_; // to create & interact
  const Scene::WeaponData* model_;

  int animation_id_ = -1;
  Animator* animator_ = nullptr;

  OperationalState op_state_ = OperationalState::Idle;

  int bullets_left_ = 0;
  int magazines_left_ = 0;

  // Only valid if PhysicsState::Dropped
  Character* owner_ = nullptr;
  JPH::BodyID jph_body_id_;
};

#endif  // WIREBOUNDWORLDCREATOR_WEAPON_H
