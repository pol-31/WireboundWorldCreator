#include "Weapon.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "Character.h"
#include "Animator.h"
#include "../../Core/Layers.h"

Weapon::Weapon(
  const Scene::WeaponData* model,
  JPH::PhysicsSystem* physics_system,
  Animator* animator)
: model_(model),
physics_system_(physics_system),
animator_(animator) {
  CreatePhysicBody(JPH::Vec3(1.0f, 1.0f, 1.0f));
}

void Weapon::CreatePhysicBody(const JPH::Vec3& position) {
  const auto& mesh = model_->meshes[0];
  auto half_extend_glm = (mesh.max - mesh.min) / 2.0f;
  JPH::Vec3 half_extend(half_extend_glm.x, half_extend_glm.y, half_extend_glm.z);
  JPH::BoxShapeSettings shape_settings(half_extend); // Approximate rifle box
  JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
  JPH::BodyCreationSettings creation_settings(
      shape_result.Get(),
      position,
      JPH::Quat::sIdentity(),
      JPH::EMotionType::Dynamic,
      Layers::MOVING
  );
  auto& bi = physics_system_->GetBodyInterface();
  jph_body_id_ = bi.CreateAndAddBody(creation_settings, JPH::EActivation::Activate);
}

void Weapon::DeletePhysicBody() {
  JPH::BodyInterface& bi = physics_system_->GetBodyInterface();
  if (!jph_body_id_.IsInvalid()) {
    bi.RemoveBody(jph_body_id_);
    bi.DestroyBody(jph_body_id_);
    jph_body_id_ = JPH::BodyID();
  }
}

//TODO 1: we don't render by dfs, but all meshes (wft - need by nodes with meshes)
//TODO 2: weapon global transforms should be updated alongside with scene objects
// (so in Scene::Update())
//TODO 3: gltf::mesh != gltf::primitive



AnimatedRenderData Weapon::GetAnimatedRenderData() const {
  AnimatedRenderData data;
  if (owner_) {
    data.transform = owner_->GetWeaponSocketMatrix();
  } else {
  auto& bli = physics_system_->GetBodyLockInterface();
    JPH::BodyLockRead lock(bli, jph_body_id_);
    if (lock.SucceededAndIsInBroadPhase()) {
      const JPH::Body& body = lock.GetBody();
      //node->bounds = body.GetWorldSpaceBounds(); // update bounds
      data.transform = body.GetWorldTransform();
    }
  }
  if (animation_id_ == -1) {
    data.bones_offset = -1;
  } else {
    data.bones_offset = animator_->GetInstanceWeapon(animation_id_).core_instance.bones_offset;
  }
  data.vao = model_->vao;
  data.material = &model_->material;
  data.meshes = &model_->meshes;
  return data;
}

bool Weapon::TryShoot(const JPH::Vec3& eye_pos, const JPH::Vec3& forward_dir) {
  // if (op_state_ != OperationalState::Idle) {
  //   return;
  // }
  // if (bullets_left_ == 0) {
  //   std::cerr << "a ni ma bullets" << std::endl;
  //   //TODO: render ui message
  //   return;
  // }
  animator_->StartWeapon(animation_id_, WeaponAnimType::Shoot, false);
  return true;
}

void Weapon::Reload() {
  animator_->StartWeapon(animation_id_, WeaponAnimType::Reload, false);
  // if (op_state_ != OperationalState::Idle) {
  //   return;
  // }
  // if (bullets_left_ == bullets_max_) {
  //   return;
  // }
  // shared_data_->animator->Start(animation_id_weapon_, Animator::WeaponType::Reload, false);
}

void Weapon::Update(float dt) {
  // if (op_state_ == OperationalState::Idle) {
  //   return;
  // }
  // const auto& animation = animator_->GetInstance(animation_id_);
  // if (animation.is_idle) {
  //   op_state_ = OperationalState::Idle;
  // }
}

void Weapon::Equip(Character* new_owner) {
  if (owner_) {
    return;
  }
  owner_ = new_owner;
  DeletePhysicBody();

  animation_id_ = animator_->AddInstanceWeapon(
    model_->nodes[model_->rig.core_rig.skeletonRoot], &model_->nodes, &model_->rig);
  animator_->StartWeapon(animation_id_, WeaponAnimType::Idle, false);
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
  animation_id_ = -1; // making it inactive
}
