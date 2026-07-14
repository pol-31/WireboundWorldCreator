#include "Character.h"

#include "../../core/Layers.h"

#include <iostream>

void Character::Death() {
  if (state_ != State::kStunned) {
    shared_data_->animator_->Start(
      animation_id_, (int)Animator::CharacterType::kStunned, false);
    state_ = State::kStunned;
  }
}

void Character::Shoot() {
  shared_data_->animator_->Start(
    animation_id_weapon_, (int)Animator::WeaponType::Shoot, false);
  shared_data_->animator_->Start(
    animation_id_, (int)Animator::CharacterType::kFall, false);
}

Character::Character(
JPH::Ref<JPH::CharacterVirtual> jph_character,
CharacterSharedData* shared_data,
const Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::Skin* skin)
  : scene_node_(scene_node),
    model_(model),
    skin_(skin),
    jph_character_(std::move(jph_character)),
    shared_data_(shared_data) {
  animation_id_ = shared_data_->animator_->AddInstance(
    model_->nodes[0], &model_->nodes, skin);
  shared_data_->mCharacterVsCharacterCollision_.Add(jph_character_);
}

void BindMaterial(const Material& material) {
  glActiveTexture(GL_TEXTURE0);
  material.albedo.BindSampler(0);
  glActiveTexture(GL_TEXTURE0);
  material.rough_metal_ao.BindSampler(1);
  glActiveTexture(GL_TEXTURE0);
  material.normal.BindSampler(2);
}

void Character::RenderWithWeapon(const Frustum& frustum) const {
  if (!frustum.Overlaps(model_->nodes[0]->bounds)) {
    // return;
  }
  auto position = scene_node_->global_transform.GetTranslation();
  position = GetPosition();
  // std::cout << position.GetX() << ' ' << position.GetY() << ' '
  // << position.GetZ() << std::endl;
  auto transform = JPH::Mat44::sIdentity();
  transform.SetTranslation(position);

  glUniformMatrix4fv(2, 1, false,
    reinterpret_cast<const float*>(&transform));
  glUniform4fv(3, 1, JPH::Vec4::sOne().mF32);
  glUniform1ui(4, shared_data_->animator_->GetInstance(animation_id_).bones_offset);

  glBindVertexArray(model_->vao);
  BindMaterial(model_->material);

  // vbo & ebo holds all data, including weights and joints
  std::function<void(const Scene::ModelNode*)> dfs =
    [&](const Scene::ModelNode* node) {
      if (node->mesh_index != -1) {
        const auto& m = model_->meshes[node->mesh_index];
        glDrawElements(GL_TRIANGLES, m.index_count,
          m.index_type, (void*)m.index_byte_offset);
      }
    for (const Scene::ModelNode* child : node->children) {
      dfs(child);
    }
  };
  dfs(model_->nodes[0]);
  for (const auto& m : model_->meshes) {
    glDrawElements(GL_TRIANGLES, m.index_count,
      m.index_type, (void*)m.index_byte_offset);
  }

  /// if we hold a weapon, we render it as well, while animation
  /// already updated through animation_id_weapon_ internally for Animator
  // if (equipped_weapon_) {
  //   const Scene::WeaponData* weapon_model = equipped_weapon_->GetModel();
  //   glBindVertexArray(weapon_model->vao);
  //   BindMaterial(weapon_model->material);
  //   auto hand_matrix = GetHandBoneMatrix();
  //   glUniformMatrix4fv(2, 1, false,
  //     reinterpret_cast<const float*>(&hand_matrix));
  //   glUniform4fv(3, 1, JPH::Vec4::sOne().mF32);
  //   glUniform1ui(4, shared_data_->animator_
  //     ->GetInstance(animation_id_weapon_).bones_offset);
  //   dfs(weapon_model->nodes[0]);
  // }
}

void Character::UpdateWalkAnimation(JPH::Vec3 velocity) {
  if (state_ == State::kIdle) {
    const auto& animation_data = shared_data_->animator_->GetInstance(animation_id_);
    auto hor_velocity = JPH::Vec3(velocity.GetX(), 0.0f, velocity.GetZ());
    auto hor_velocity_mag = hor_velocity.Length();
    bool is_moving = hor_velocity_mag > 1.0f;
    Animator::CharacterType new_type;
    if (is_moving) {
      if (hor_velocity_mag < default_speed_ + 1.0f) {
        new_type = Animator::CharacterType::kWalk;
      } else {
        new_type = Animator::CharacterType::kRun;
      }
      // new_animationd_id = HumanAnimation::kCrouch;
    } else {
      new_type = Animator::CharacterType::kIdle;
    }
    if (animation_data.type != static_cast<int>(new_type)) {
      shared_data_->animator_->Start(animation_id_, (int)new_type, true);
    }
  }
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
void Character::PostPhysicsUpdate(JPH::Vec3 look_forward, JPH::Vec3 gravity, float dt) {

  // Rotate controls to align with the camera
  JPH::Vec3 cam_fwd = look_forward;
  cam_fwd.SetY(0.0f);
  cam_fwd = cam_fwd.NormalizedOr(JPH::Vec3::sAxisX());
  JPH::Quat rotation = JPH::Quat::sFromTo(JPH::Vec3::sAxisX(), cam_fwd);

  JPH::Vec3 world_movement_direction = rotation * movement_direction_;
  JPH::Vec3 mDesiredVelocity = JPH::Vec3::sZero();

  if (jph_character_->IsSupported()) {
    mDesiredVelocity = world_movement_direction * speed_;
    mAllowSliding = !world_movement_direction.IsNearZero();
  } else {
    mAllowSliding = true;
  }
  JPH::Quat character_up_rotation = JPH::Quat::sEulerAngles(JPH::Vec3(0, 0, 0));
  jph_character_->SetUp(character_up_rotation.RotateAxisY());
  JPH::Quat target_rotation = JPH::Quat::sFromTo(JPH::Vec3::sAxisZ(), cam_fwd);
  jph_character_->SetRotation(target_rotation);

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
      new_velocity += 1 * jump_speed_  * jph_character_->GetUp();
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
  jph_character_->SetLinearVelocity(new_velocity);

  UpdateWalkAnimation(new_velocity);
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
  JPH::Mat44 bone_model_space = shared_data_->animator_
  ->GetNodeGlobalTransform(animation_id_, skin_->hand_bone_id);
  JPH::Mat44 character_world = JPH::Mat44::sRotationTranslation(
    GetRotation(), GetPosition());
  return character_world * bone_model_space;
}

JPH::Mat44 Character::GetCameraBoneMatrix() const {
  JPH::Mat44 head_model_space = shared_data_->animator_
  ->GetNodeGlobalTransform(animation_id_, skin_->head_bone_id);
  JPH::Mat44 character_world = JPH::Mat44::sRotationTranslation(
    GetRotation(), GetPosition());

  return character_world * head_model_space;
}
