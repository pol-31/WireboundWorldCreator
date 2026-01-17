#include "RigidBody.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../Details.h"
#include "../../io/Window.h"  // gDeltaTime

float GetYawFromQuat(const glm::quat& q) {
  return std::atan2(2.0f * (q.y * q.w + q.x * q.z),
                    1.0f - 2.0f * (q.y * q.y + q.x * q.x));
}

void RigidBody::RenderPicking(UiSharedResources& ui_shared_resources) {
  //TODO: render capsule model instead
  UpdateAnimation();
  ui_shared_resources.shader_animated_mdl_picking_.Bind();
  glUniform1ui(1, static_cast<uint32_t>(details::kIdOffsetObjects + id_));
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

void RigidBody::UpdatePosition(UiSharedResources& ui_shared_resources,
                               glm::vec3 position_diff) {
  glm::vec2 pos_start(position_.x, position_.z);
  position_ += position_diff * speed_ * gDeltaTime;
  position_ = glm::clamp(position_, -32.0f, 32.0f);
  glm::vec2 pos_end(position_.x, position_.z);
  UpdatePositionY(ui_shared_resources);
  glm::vec2 move_dir = pos_end - pos_start;

  if (glm::length(move_dir) < 0.0001f) {
    return;  // no movement
  }

  float angle_start = GetYawFromQuat(rotation_);
  float angle_move = std::atan2(move_dir.x, move_dir.y);

  float delta = angle_move - angle_start;
  delta = std::remainder(delta, glm::two_pi<float>());
  //  delta = std::fmod(delta + glm::pi<float>(), glm::two_pi<float>()) -
  //  glm::pi<float>();

  float factor_lag = 10.0f * gDeltaTime;  // for lerp
  float new_yaw = angle_start + delta * factor_lag;

  rotation_ = glm::angleAxis(new_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 RigidBody::GenModelMat(UiSharedResources& ui_shared_resources,
                                 float scale) {
  auto map_scale =
      ui_shared_resources.glfw_context_.tile_renderer->cur_tile_.map_scale;
  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(object_model, position_);
  object_model *= glm::mat4_cast(rotation_);
  object_model = glm::scale(object_model, scale_ * scale);
  glm::mat4 map_model =
      glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));  // upscaled
  return map_model * object_model;
}

void RigidBody::UpdatePositionY(UiSharedResources& ui_shared_resources) {
  float ground_height = ui_shared_resources.glfw_context_.tile_renderer
    ->cur_tile_.GetPositionY(position_.x * 16.0f + 512.0f,
                             position_.z * 16.0f + 512.0f);
  if (!IsOnGround()) {
    const float gravity = -9.81f;
    const float mass = 4.0f;
    gravity_velocity_ += gDeltaTime * gravity;
    position_.y += gravity_velocity_ * gDeltaTime * mass;
    if (position_.y <= ground_height) {
      position_.y = ground_height;
      gravity_velocity_ = 0.0f;
      ResetState();
    }
  } else {
    position_.y = ground_height;
  }
}

Aabb3D RigidBody::GenWorldAabb() const noexcept {
  auto model_mat = glm::translate(glm::mat4(1.0f), GetPosition());
  model_mat = glm::scale(model_mat, GetScale());
  return Aabb3D::TransformAabb(model_data_->aabb, model_mat);
}

void RigidBody::Jump(float strength) {
  if (IsRelaxed()) {
    gravity_velocity_ = strength;
    state_ = State::kJumping;
    animation_time_ = 0.0f;
    animation_id_ = HumanAnimator::Type::kJump;
    animation_looped_ = false;
  }
}

void RigidBody::Kick() {
  if (IsRelaxed()) {
    state_ = State::kAttacking;
    animation_time_ = 0.0f;
    animation_id_ = HumanAnimator::Type::kKick;
    animation_looped_ = false;
    AttackEvent attack;
    attack.attacker = id_;
    attack.damage = 1.0f;
    attack.hitbox = GenWorldAabb();
    attack_queue_->push_back(attack);
  }
}

void RigidBody::Stunned() {
  if (IsRelaxed()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = HumanAnimator::Type::kStunned;
    animation_looped_ = false;
  }
}

void RigidBody::Fall() {
  if (state_ != State::kFalling) {
    state_ = State::kFalling;
    animation_time_ = 0.0f;
    animation_id_ = HumanAnimator::Type::kFall;
    animation_looped_ = true;
  }
}

void RigidBody::ApplyGravity(UiSharedResources& ui_shared_resources) {
  float ground_height = ui_shared_resources.glfw_context_.tile_renderer
    ->cur_tile_.GetPositionY(position_.x * 16.0f + 512.0f,
                             position_.z * 16.0f + 512.0f);
  if (!IsOnGround()) {
    const float gravity = -9.81f;
    const float mass = 1.0f;
    gravity_velocity_ += gDeltaTime * gravity;
    position_.y += gravity_velocity_ * gDeltaTime * mass;
    if (position_.y <= ground_height) {
      position_.y = ground_height;
      gravity_velocity_ = 0.0f;
      ResetState();
    } else {
      if (gravity_velocity_ <= 0.0f) {
        Fall();
      }
    }
  } else {
    position_.y = ground_height;
  }
}

void RigidBody::ResetState() {
  state_ = State::kIdle;
  animation_id_ = HumanAnimator::Type::kIdle;
  animation_time_ = 0.0f;
  animation_looped_ = true;
}

// should be called in Render(), it updates skin ubo
void RigidBody::UpdateAnimation() {
  bool started_over = animator_->UpdateUbo(animation_id_, animation_time_);
  if (started_over && !animation_looped_) {
    ResetState();
  }
}
