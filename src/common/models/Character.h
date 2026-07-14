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
  Character(
JPH::Ref<JPH::CharacterVirtual> jph_character,
CharacterSharedData* shared_data,
const Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::Skin* skin);

  void RenderWithWeapon(const Frustum& frustum) const;

  void Run() {
    speed_ *= 4.0f;
    SwitchStance(true);
  }

  void Crouch() {
    speed_ *= 0.5f;
    SwitchStance(false);
  }

  void TriggerJump() {
    jump_triggered_ = true;
  }

  /// in case of Run() or Crouch()
  void BackToWalk() {
    speed_ = default_speed_;
    SwitchStance(true);
  }

  void TakeDamage(float amount) {
    health_ -= amount;
    if (health_ <= 0.0f) {
      Death();
    }
  }

  void EquipWeapon(Weapon* weapon) {
    equipped_weapon_ = weapon;
    equipped_weapon_->Equip(this);
    const Scene::WeaponData* data = weapon->GetModel();
    animation_id_weapon_ = shared_data_->animator_->AddInstance(
      data->nodes[0], &data->nodes, &data->rig);
  }

  void Disarm() {
    animation_id_weapon_ = -1;
    equipped_weapon_->Drop(GetPosition(), JPH::Vec3::sZero());
    equipped_weapon_ = nullptr;
  }

  const Scene::CharacterData* GetModel() const { return model_; }
  const Scene::Skin* GetSkin() const { return skin_; }

  JPH::Mat44 GetHandBoneMatrix() const;
  JPH::Mat44 GetCameraBoneMatrix() const;

  enum class State {
    kIdle, // stand, walk, run
    kStunned // can't do anything
  };

  void Death();

  void UpdateWalkAnimation(JPH::Vec3 velocity);

  void Shoot();

  void PrePhysicsUpdate(
    const JPH::PhysicsSystem* physics_system,
    JPH::TempAllocator* temp_allocator, float dt);

   void PostPhysicsUpdate(JPH::Vec3 look_forward, JPH::Vec3 gravity, float dt);

  bool IsStanding();

  void SwitchStance(bool do_stand);

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

private:
  friend class EnemyController;
  friend class PlayerController;

  const Scene::ModelNode* scene_node_;
  const Scene::CharacterData* model_ = nullptr;
  const Scene::Skin* skin_ = nullptr;
  CharacterSharedData* shared_data_ = nullptr;
  Weapon* equipped_weapon_ = nullptr;
  JPH::Ref<JPH::CharacterVirtual> jph_character_ = nullptr;

  State state_ = State::kIdle;
  int animation_id_ = -1;
  int animation_id_weapon_ = -1;

  JPH::Vec3 external_impulse_ = JPH::Vec3::sZero();
  bool jump_triggered_ = false;
  JPH::Vec3 movement_direction_ = JPH::Vec3(0.0f, 0.0f, 0.0f);
  JPH::Vec3 look_direction_ = JPH::Vec3(0.0f, 0.0f, 0.0f);

  float health_ = 100.0f;
  float default_speed_ = 3.0f;
  float speed_ = default_speed_;
  float jump_speed_ = 4.0f;
  bool mAllowSliding = false;
};

#endif  // WIREBOUNDWORLDCREATOR_CHARACTER_H
