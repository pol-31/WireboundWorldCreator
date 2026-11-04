#include "Weapon.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include "miniaudio.h"

#include "../../Core/Layers.h"
#include "Animator.h"
#include "Character.h"

Weapon::Weapon(const Scene::WeaponData* model,
               JPH::PhysicsSystem* physics_system, Animator* animator,
               ma_engine* audio_engine, ma_sound_group* sfx_group)
    : model_(model),
physics_system_(physics_system),
animator_(animator),
audio_engine_(audio_engine),
sfx_group_(sfx_group) {
  CreatePhysicBody(JPH::Vec3(1.0f, 1.0f, 1.0f));
  ma_sound_init_from_file(audio_engine_, "../assets/audio/sound_gun_shoot.mp3", 0, sfx_group_, NULL, &sound_shoot_);
  ma_sound_init_from_file(audio_engine_, "../assets/audio/sound_gun_reload.mp3", 0, sfx_group_, NULL, &sound_reload_);
  ma_sound_init_from_file(audio_engine_, "../assets/audio/sound_gun_pickup.mp3", 0, sfx_group_, NULL, &sound_pickup_);
}

void Weapon::CreatePhysicBody(const JPH::Vec3& position) {
  const auto& mesh = model_->meshes[0];
  auto half_extend_glm = (mesh.max - mesh.min) / 2.0f;
  JPH::Vec3 half_extend(half_extend_glm.x, half_extend_glm.y,
                        half_extend_glm.z);
  JPH::BoxShapeSettings shape_settings(half_extend);  // Approximate rifle box
  JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
  JPH::BodyCreationSettings creation_settings(
      shape_result.Get(), position, JPH::Quat::sIdentity(),
      JPH::EMotionType::Dynamic, Layers::MOVING);
  auto& bi = physics_system_->GetBodyInterface();
  jph_body_id_ =
      bi.CreateAndAddBody(creation_settings, JPH::EActivation::Activate);
}

void Weapon::DeletePhysicBody() {
  JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
  if (!jph_body_id_.IsInvalid()) {
    bi.RemoveBody(jph_body_id_);
    bi.DestroyBody(jph_body_id_);
    jph_body_id_ = JPH::BodyID();
  }
}

// TODO 1: we don't render by dfs, but all meshes (wft - need by nodes with
// meshes)
// TODO 2: weapon global transforms should be updated alongside with scene
// objects
//  (so in Scene::Update())
// TODO 3: gltf::mesh != gltf::primitive

JPH::Vec3 Weapon::GetPosition() const {
  JPH::Vec3 transform = JPH::Vec3::sZero();
  if (owner_) {
    transform = owner_->GetPosition();
  } else {
    auto& bli = physics_system_->GetBodyLockInterface();
    JPH::BodyLockRead lock(bli, jph_body_id_);
    if (lock.SucceededAndIsInBroadPhase()) {
      const JPH::Body& body = lock.GetBody();
      transform = body.GetPosition();
    }
  }
  return transform;
}

AnimatedRenderData Weapon::GetAnimatedRenderData() const {
  AnimatedRenderData data;
  if (owner_) {
    data.transform = owner_->GetWeaponSocketMatrix();
  } else {
    auto& bli = physics_system_->GetBodyLockInterface();
    JPH::BodyLockRead lock(bli, jph_body_id_);
    if (lock.SucceededAndIsInBroadPhase()) {
      const JPH::Body& body = lock.GetBody();
      // node->bounds = body.GetWorldSpaceBounds(); // update bounds
      data.transform = body.GetWorldTransform();
    }
  }
  if (animation_id_ == -1) {
    data.bones_offset = -1;
  } else {
    data.bones_offset =
        animator_->GetInstanceWeapon(animation_id_).core_instance.bones_offset;
  }
  data.vao = model_->vao;
  data.material = &model_->material;
  data.meshes = &model_->meshes;
  return data;
}

bool Weapon::TryShoot(const JPH::Vec3& eye_pos, const JPH::Vec3& forward_dir) {
  if (op_state_ != OperationalState::Idle) {
    return false;
  }
  op_state_ = OperationalState::Shoot;
  // if (bullets_left_ == 0) {
  //   std::cerr << "a ni ma bullets" << std::endl;
  //   //TODO: render ui message
  //   return;
  // }
  // auto position = GetPosition();
  // ma_sound_set_position(&sound_shoot_, position.GetX(), position.GetY(), position.GetZ());
  ma_sound_stop(&sound_shoot_); // TODO: sound too long now
  ma_sound_seek_to_pcm_frame(&sound_shoot_, 0);
  ma_sound_start(&sound_shoot_);
  animator_->StartWeapon(animation_id_, WeaponAnimType::Shoot, false);
  return true;
}

void Weapon::Reload() {
  if (op_state_ != OperationalState::Idle) {
    return;
  }
  // auto position = GetPosition();
  // ma_sound_set_position(&sound_reload_, position.GetX(), position.GetY(), position.GetZ());
  ma_sound_start(&sound_reload_);
  animator_->StartWeapon(animation_id_, WeaponAnimType::Reload, false);
  op_state_ = OperationalState::Reload;
  // if (bullets_left_ == bullets_max_) {
  //   return;
  // }
  // shared_data_->animator->Start(animation_id_weapon_,
  // Animator::WeaponType::Reload, false);
}

void Weapon::Update(float dt) {
  auto position = GetPosition();
  ma_sound_set_position(&sound_shoot_, position.GetX(), position.GetY(), position.GetZ());
  ma_sound_set_position(&sound_reload_, position.GetX(), position.GetY(), position.GetZ());
  ma_sound_set_position(&sound_pickup_, position.GetX(), position.GetY(), position.GetZ());
  if (op_state_ == OperationalState::Idle) {
    return;
  }
  const auto& animation = animator_->GetInstanceWeapon(animation_id_);
  if (animation.core_instance.is_idle) {
    op_state_ = OperationalState::Idle;
  }
}

void Weapon::Equip(Character* new_owner) {
  if (owner_) {
    return;
  }
  owner_ = new_owner;
  DeletePhysicBody();

  animation_id_ = animator_->AddInstanceWeapon(
      model_->nodes[model_->rig.core_rig.skeletonRoot], &model_->nodes,
      &model_->rig);
  animator_->StartWeapon(animation_id_, WeaponAnimType::Idle, false);
  // auto position = GetPosition();
  // ma_sound_set_position(&sound_pickup_, position.GetX(), position.GetY(), position.GetZ());
  ma_sound_start(&sound_pickup_);
}

void Weapon::Drop(const JPH::Vec3& position, const JPH::Vec3& impulse) {
  if (!owner_) {
    return;
  }
  owner_ = nullptr;
  CreatePhysicBody(position);
  JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
  bi.SetLinearVelocity(jph_body_id_, impulse);
  animator_->RemoveInstanceWeapon(animation_id_);
  animation_id_ = -1;  // making it inactive
}
