#include "Character.h"

#include <iostream>

#include "../../core/Layers.h"
#include "Animator.h"

const float Character::cWalkSpeed = 3.0f;
const float Character::cRunSpeed = 12.0f;
const float Character::cJumpSpeed = 4.0f;

void Character::Death() {
  if (state_ != State::kStunned) {
    shared_data_->animator_->StartCharacter(
      animation_id_, CharacterAnimType::Hurt, false);
    state_ = State::kStunned;
  }
}

void Character::Shoot(JPH::Vec3 pos, JPH::Vec3 dir) {
  if (!equipped_weapon_) {
    return;
  }
  if (!equipped_weapon_->TryShoot(JPH::Vec3::sZero(), JPH::Vec3::sZero())) {
    return;
  }
  shared_data_->Shoot(pos, dir, jph_character_->GetInnerBodyID());
}

void Character::Reload() {
  if (!equipped_weapon_) {
    return;
  }
  equipped_weapon_->Reload();
}

JPH::Vec3 Character::GetLinearVelocity() const noexcept {
  return jph_character_->GetLinearVelocity();
}

JPH::Ref<JPH::CharacterVirtual> Character::CreateJphCharacter(
  Scene::ModelNode* node) {
  JPH::Ref<JPH::CharacterVirtualSettings> settings =
    shared_data_->GetDefaultJphSettings();
  auto npc_pos = JPH::RVec3::sZero();
  JPH::Ref<JPH::CharacterVirtual> character = new JPH::CharacterVirtual(
      settings, npc_pos + JPH::RVec3(0, 0 + 1, 0),
      JPH::Quat::sIdentity(), 0, shared_data_->physics_system_);
  node->body_id = character->GetInnerBodyID();
  node->shape = settings->mShape;
  //JPH::Color color = DefineColor(body.GetMotionType(), body.GetID());
  return character;
}

Character::Character(
CharacterSharedData* shared_data,
Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::CharacterRig* skin)
  : scene_node_(scene_node),
    model_(model),
    skin_(skin),
    shared_data_(shared_data) {
  jph_character_ = CreateJphCharacter(scene_node);
  animation_id_ = shared_data_->animator_->AddInstanceCharacter(
    model_->nodes[skin->core_rig.skeletonRoot], &model_->nodes, skin, this);
  shared_data_->mCharacterVsCharacterCollision_.Add(jph_character_);
}

JPH::Quat Character::GetRenderRotation() const noexcept {
  /// we already in the bone space; simply need the overall body render rotation
  /// (what is animation body rotation)
  /// final render yaw (bone upper body already blended INTO the rig globals)
  float yaw = shared_data_->animator_->GetInstanceCharacter(animation_id_).body_yaw_;
  return JPH::Quat::sRotation(JPH::Vec3::sAxisY(), JPH::DegreesToRadians(yaw)).Normalized();
}

AnimatedRenderData Character::GetAnimatedRenderData() const {
  AnimatedRenderData data;
  data.transform = JPH::Mat44::sRotationTranslation(GetRenderRotation(), GetPosition());
  data.bones_offset = shared_data_->animator_->GetInstanceCharacter(animation_id_).core_instance.bones_offset;
  data.vao = model_->vao;
  data.material = &model_->material;
  data.meshes = &model_->meshes;
  return data;
}

void Character::PrePhysicsUpdate(
    const JPH::PhysicsSystem* physics_system,
    JPH::TempAllocator* temp_allocator, float dt) {
  auto character_up = jph_character_->GetUp();
  JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;
  update_settings.mStickToFloorStepDown = -character_up * update_settings.mStickToFloorStepDown.Length();
  update_settings.mWalkStairsStepUp = character_up * update_settings.mWalkStairsStepUp.Length();
  jph_character_->ExtendedUpdate(dt,
                -character_up * physics_system->GetGravity().Length(),
                update_settings,
                physics_system->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
                physics_system->GetDefaultLayerFilter(Layers::MOVING),
                { },
                { },
                *temp_allocator);
}

// input dir
// cur walk dir
// look dir

void Character::PostPhysicsUpdate(JPH::Vec3 gravity, float dt) {
  // 1. Calculate Target Speed based on input
  float target_speed = 0.0f;
  if (movement_direction_.Length() > 0.1f) {
    target_speed = is_sprinting_ ? cRunSpeed : cWalkSpeed;
  }

  // 2. Snappy Exponential Acceleration (The "Logarithmic" feel)
  // A high sharpness (e.g., 10.0f - 15.0f) means it reaches target speed almost instantly
  // but still has a micro-blend to prevent snapping the animation.
  float sharpness = 12.0f;
  speed_ += (target_speed - speed_) * (1.0f - std::exp(-sharpness * dt));

  // Clamp to avoid floating point drift
  speed_ = std::clamp(speed_, 0.0f, cRunSpeed);

  //TODO: movement_direction might me 0, so
  // then we skip the process and the animations as well

  JPH::Vec3 world_movement_direction = movement_direction_;
  JPH::Vec3 mDesiredVelocity = JPH::Vec3::sZero();

  if (jph_character_->IsSupported()) {
    mDesiredVelocity = world_movement_direction * speed_;
    mAllowSliding = !world_movement_direction.IsNearZero();
  } else {
    mAllowSliding = true;
  }
  JPH::Quat character_up_rotation = JPH::Quat::sEulerAngles(JPH::Vec3(0, 0, 0));
  jph_character_->SetUp(character_up_rotation.RotateAxisY());
  // we do not rotate the capsule
  // JPH::Vec3 cam_fwd = head_facing_;
  // cam_fwd.SetY(0.0f);
  // cam_fwd = cam_fwd.NormalizedOr(JPH::Vec3::sAxisX());
  // JPH::Quat target_rotation = JPH::Quat::sFromTo(JPH::Vec3::sAxisZ(), cam_fwd);
  // jph_character_->SetRotation(target_rotation);

  jph_character_->UpdateGroundVelocity();
  JPH::Vec3 current_vertical_velocity = jph_character_->GetLinearVelocity().Dot(jph_character_->GetUp()) * jph_character_->GetUp();
  JPH::Vec3 ground_velocity = jph_character_->GetGroundVelocity();
  JPH::Vec3 new_velocity;
  bool moving_towards_ground = (current_vertical_velocity.GetY() - ground_velocity.GetY()) < 0.1f;
  if (jph_character_->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround &&
      (!jph_character_->IsSlopeTooSteep(jph_character_->GetGroundNormal()))) {
    new_velocity = ground_velocity;
    if (state_ != State::kIdle) {
      state_ = State::kIdle;
    }
    if (jump_triggered_ && moving_towards_ground) {
      new_velocity += 1.5f * cJumpSpeed * jph_character_->GetUp();
      jump_triggered_ = false;
      state_ = State::kStunned;
    }
  } else
    new_velocity = current_vertical_velocity;

  // Gravity
  new_velocity += (character_up_rotation * gravity) * dt;

  if (jph_character_->IsSupported()) {
    new_velocity += character_up_rotation * mDesiredVelocity;
  } else {
    JPH::Vec3 current_horizontal_velocity = jph_character_->GetLinearVelocity() - current_vertical_velocity;
    new_velocity += current_horizontal_velocity;
  }
  // std::cout << new_velocity.GetX() << ' ' << new_velocity.GetX() << std::endl;
  jph_character_->SetLinearVelocity(new_velocity);
}

bool Character::IsStanding() {
  return jph_character_->GetShape() == shared_data_->mStandingShape_;
}

void Character::SwitchStance(bool do_stand) {
  bool is_standing = IsStanding();
  const JPH::Shape *shape = is_standing? shared_data_->mCrouchingShape_ : shared_data_->mStandingShape_;
  if ((do_stand && is_standing) || (!do_stand && !is_standing)) {
    return; // already needed stance
  }
  if (jph_character_->SetShape(shape, 1.5f * shared_data_->physics_system_->GetPhysicsSettings()
    .mPenetrationSlop, shared_data_->physics_system_->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
    shared_data_->physics_system_->GetDefaultLayerFilter(Layers::MOVING),
    { }, { }, *shared_data_->temp_allocator_)) {
    const JPH::Shape *inner_shape = IsStanding()? shared_data_->mInnerCrouchingShape_ : shared_data_->mInnerStandingShape_;
    jph_character_->SetInnerBodyShape(inner_shape);
  }
}

JPH::Vec3 Character::GetPosition() const {
  auto& body_interface = shared_data_->physics_system_->GetBodyInterface();
  auto id = jph_character_->GetInnerBodyID();
  return body_interface.GetPosition(id);
}

JPH::Quat Character::GetRotation() const {
  auto& body_interface = shared_data_->physics_system_->GetBodyInterface();
  auto id = jph_character_->GetInnerBodyID();
  return body_interface.GetRotation(id);
}
JPH::Mat44 Character::GetHandBoneMatrix() const {
  // return JPH::Mat44::sIdentity();
  JPH::Mat44 bone_model_space = shared_data_->animator_
  ->GetNodeGlobalTransformCharacter(animation_id_, skin_->hand_bone_id);
  JPH::Mat44 character_world = JPH::Mat44::sRotationTranslation(GetRenderRotation(), GetPosition());
  return character_world * bone_model_space;
}

JPH::Mat44 Character::GetCameraBoneMatrix() const {
  JPH::Mat44 head_model_space = shared_data_->animator_
  ->GetNodeGlobalTransformCharacter(animation_id_, skin_->head_bone_id);
  JPH::Mat44 character_world = JPH::Mat44::sRotationTranslation(GetRenderRotation(), GetPosition());
  return character_world * head_model_space;
}

void Character::Run() {
  is_sprinting_ = true;
    //speed_ *= 4.0f;
    SwitchStance(true);
  }

  void Character::Crouch() {
    //speed_ *= 0.5f;
    is_crouch_ = true;
    SwitchStance(false);
  }

  void Character::BackToWalk() {
  is_sprinting_ = false;
    is_crouch_ = false;
    //speed_ = cWalkSpeed;
    SwitchStance(true);
  }

  void Character::TakeDamage(float amount) {
    health_ -= amount;
    if (health_ <= 0.0f) {
      Death();
    }
  }

void Character::EquipWeapon(Weapon* weapon) {
  if (equipped_weapon_) {
    Disarm();
  }
  equipped_weapon_ = weapon;
  equipped_weapon_->Equip(this);
}

void Character::Disarm() {
  if (!equipped_weapon_) {
    return;
  }
  equipped_weapon_->Drop(GetPosition(), JPH::Vec3::sZero());
  equipped_weapon_ = nullptr;
}

JPH::Mat44 Character::GetWeaponSocketMatrix() const {
  // 1. Get the full matrix (contains the 90-deg rotation AND the 0.01 scale)
  JPH::Mat44 bone_model_space = shared_data_->animator_
      ->GetNodeGlobalTransformCharacter(animation_id_, skin_->hand_bone_id);

  // 2. Normalize the axes.
  // This DESTROYS the 0.01 scale but KEEPS the 90-deg rotation.
  JPH::Vec3 x = bone_model_space.GetAxisX().Normalized();
  JPH::Vec3 y = bone_model_space.GetAxisY().Normalized();
  JPH::Vec3 z = bone_model_space.GetAxisZ().Normalized();
  JPH::Vec3 translation = bone_model_space.GetTranslation();

  // 3. Rebuild the clean matrix (Scale is now exactly 1.0)
  JPH::Mat44 clean_socket_space = JPH::Mat44(
      JPH::Vec4(x, 0.0f),
      JPH::Vec4(y, 0.0f),
      JPH::Vec4(z, 0.0f),
      JPH::Vec4(translation, 1.0f)
  );

  JPH::Mat44 character_world = JPH::Mat44::sRotationTranslation(GetRenderRotation(), GetPosition());

  return character_world * clean_socket_space;
}