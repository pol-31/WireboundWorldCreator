#ifndef WIREBOUNDWORLDCREATOR_CHARACTER_H
#define WIREBOUNDWORLDCREATOR_CHARACTER_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "../../core/Frustum.h"
#include "Scene.h"
#include "Weapon.h"
#include "CharacterSharedData.h"


constexpr bool sPlayerCanPushOtherCharacters = true;
constexpr bool sOtherCharactersCanPushPlayer = true;

class Character {
public:
  enum class State {
    kIdle, // stand, walk, run
    kStunned // can't do anything
  };

  [[nodiscard]] bool IsIdle() const noexcept {
    return state_ == State::kIdle;
  }

  Character(
CharacterSharedData* shared_data,
Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::CharacterRig* skin);

  AnimatedRenderData GetAnimatedRenderData() const;

  void Run();

  void Crouch();

  /// in case of Run() or Crouch()
  void BackToWalk();

  void TakeDamage(float amount);

  void Death();

  /// we create weapon as a separate scene object
  void EquipWeapon(Weapon* weapon);

  void Disarm();

  void Shoot(JPH::Vec3 pos, JPH::Vec3 dir);

  void Reload();

  void PrePhysicsUpdate(
    const JPH::PhysicsSystem* physics_system,
    JPH::TempAllocator* temp_allocator, float dt);

   void PostPhysicsUpdate(JPH::Vec3 gravity, float dt);

  bool IsStanding();

  void SwitchStance(bool do_stand);

  const Scene::CharacterData* GetModel() const { return model_; }
  const Scene::CharacterRig* GetSkin() const { return skin_; }

  JPH::Mat44 GetHandBoneMatrix() const;
  JPH::Mat44 GetCameraBoneMatrix() const;
  JPH::Mat44 GetWeaponSocketMatrix() const;

  JPH::Vec3 GetPosition() const;

  JPH::Quat GetRotation() const;

  JPH::CharacterVirtual* GetJphCharacter() {
    return jph_character_;
  }

  void AllowSliding(bool val) {
    mAllowSliding = val;
  }

  bool GetAllowSliding() const {
    return mAllowSliding;
  }

  JPH::Vec3 GetLinearVelocity() const noexcept;

  float GetHeadYaw() const noexcept {
    return head_yaw_;
  }

  float GetHeadPitch() const noexcept {
    return head_pitch_;
  }

  // so you should use this rather than LinearVelocity, because it's the input,
  // while linear velocity is the consequence... idk, maybe
  JPH::Vec3 GetMoveDirection() const noexcept {
    return movement_direction_;
  }

  // for animation blending (e.g. forward - strafe_left blending)
  JPH::Vec3 GetMoveDirectionRaw() const noexcept {
     return movement_direction_raw_;
  }

  [[nodiscard]] int GetAnimationId() const noexcept {
    return animation_id_;
  }

  JPH::Quat GetRenderRotation() const noexcept;

  [[nodiscard]] bool IsAiming() const noexcept {
    return is_aiming_;
  }

  [[nodiscard]] bool IsSprinting() const noexcept {
    return is_sprinting_;
  }

  [[nodiscard]] float GetSpeed() const noexcept {
    return speed_;
  }

  static const float cWalkSpeed;
  static const float cRunSpeed;
  static const float cJumpSpeed;

private:
  friend class EnemyController;
  friend class PlayerController;

  JPH::Ref<JPH::CharacterVirtual> CreateJphCharacter(Scene::ModelNode* node);

  const Scene::ModelNode* scene_node_;
  const Scene::CharacterData* model_ = nullptr;
  const Scene::CharacterRig* skin_ = nullptr;
  CharacterSharedData* shared_data_ = nullptr;
  Weapon* equipped_weapon_ = nullptr;
  JPH::Ref<JPH::CharacterVirtual> jph_character_ = nullptr;

  State state_ = State::kIdle;
  int animation_id_ = -1;

  JPH::Vec3 external_impulse_ = JPH::Vec3::sZero();
  bool jump_triggered_ = false;

  // where to go (player controller input + camera OR enemy controller mover);
  // might me 0 as well (if no input or disabled mover)
  JPH::Vec3 movement_direction_ = JPH::Vec3(0.0f, 0.0f, 0.0f);
  JPH::Vec3 movement_direction_raw_ = JPH::Vec3(0.0f, 0.0f, 0.0f);

  bool is_sprinting_ = false;
  bool is_aiming_ = false;

  float head_yaw_ = 0.0f; // camera or mover
  float head_pitch_ = 0.0f; // camera of mover

  float health_ = 100.0f;
  float speed_ = 0.0f;
  bool mAllowSliding = false;
};

#endif  // WIREBOUNDWORLDCREATOR_CHARACTER_H
