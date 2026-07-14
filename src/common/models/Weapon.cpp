#include "Weapon.h"

void Weapon::Render(const Frustum& frustum) {
  //TODO: we don't have updated global transform or scene node
  // we also don't have node hierarchy in case of non-owned...
  // .... not ready
  if (owner_) {
    return;
  }
  //TODO: check frustum and render non-animated
}

void Weapon::Shoot(const JPH::Vec3& eye_pos, const JPH::Vec3& forward_dir) {
  // if (op_state_ != OperationalState::Idle) {
  //   return;
  // }
  // if (bullets_left_ == 0) {
  //   std::cerr << "a ni ma bullets" << std::endl;
  //   //TODO: render ui message
  //   return;
  // }
  // animator_->Start(animation_id_, (int)Animator::WeaponType::Shoot, false);
  // JPH::RVec3 hit_position;
  // JPH::BodyID hit_body_id;
  // float hit_fraction = 1.0f;
  // float maxDistance = 100.0f;
  // if (CastProbe(maxDistance, hit_fraction, hit_position, hit_body_id)) {
  //   float shotForce = 50.0f;
  //   JPH::Vec3 impulse = ToJph(camera_.GetDirectionFront()) * shotForce;
  //   JPH::BodyInterface &bi = mPhysicsSystem->GetBodyInterface();
  //   bool is_character = false;
  //   //TODO: probably there's a better way
  //   for (auto& c : characters_) {
  //     if (c.GetJphCharacter()->GetInnerBodyID() == hit_body_id) {
  //       is_character = true;
  //       float stopping_power = 15.0f;
  //       c.Death();
  //       // c.external_impulse += ToJph(camera_.GetDirectionFront()) * stopping_power;
  //     }
  //   }
  //   if (!is_character) {
  //     bi.AddImpulse(hit_body_id, impulse, hit_position);
  //   }
  // }
  //TODO: cast a ray
}

void Weapon::Reload() {
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
  owner_ = new_owner;
  phys_state_ = PhysicsState::Carried;

  // Remove the weapon from the physics simulation so it doesn't collide with the player
  if (!jph_body_id_.IsInvalid()) {
    body_interface_.RemoveBody(jph_body_id_);
    body_interface_.DestroyBody(jph_body_id_);
    jph_body_id_ = JPH::BodyID(); // Reset
  }
}

void Weapon::Drop(const JPH::Vec3& position, const JPH::Vec3& impulse) {
  owner_ = nullptr;
  phys_state_ = PhysicsState::Dropped;

  // // Create the Jolt body on the ground
  // JPH::BoxShapeSettings shape_settings(JPH::Vec3(0.4f, 0.1f, 0.05f)); // Approximate rifle box
  // JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
  //
  // JPH::BodyCreationSettings creation_settings(
  //     shape_result.Get(),
  //     ToJph(position),
  //     JPH::Quat::sIdentity(),
  //     JPH::EMotionType::Dynamic,
  //     Layers::MOVING
  // );
  //
  // jph_body_id_ = body_interface_.CreateAndAddBody(creation_settings, JPH::EActivation::Activate);
  // body_interface_.SetLinearVelocity(jph_body_id_, ToJph(impulse));
}
