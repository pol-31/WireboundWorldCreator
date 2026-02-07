#include "Human.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../../io/Window.h"  // gDeltaTime
#include "../Details.h"
#include "ModelLoader.h"

void Human::Render(float map_scale) {
  UpdateAnimation();
  model_data_->BindTextures();
  auto model = GenModelMat(map_scale);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
}

void Human::Update(UiRenderData& render_data) {
  UpdatePosition(render_data, glm::vec3{0.0f});
}

// should be called in Render(), it updates skin ubo
void Human::UpdateAnimation() {
  bool started_over = animator_->UpdateUbo(animation_id_, animation_time_);
  if (started_over && !animation_looped_) {
    ResetState();
  }
}

void Human::UpdatePosition(UiRenderData& render_data,
                           glm::vec3 position_diff) {
  glm::vec2 pos_start(position_.x, position_.z);
  position_ += position_diff * speed_ * gDeltaTime;
  position_ = glm::clamp(position_, -32.0f, 32.0f);
  glm::vec2 pos_end(position_.x, position_.z);
  UpdatePositionY(render_data);
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

void Human::UpdatePositionY(UiRenderData& render_data) {
  float ground_height =
      render_data.glfw_context_.tile_renderer->cur_tile_.GetPositionY(
          position_.x * 16.0f + 512.0f, position_.z * 16.0f + 512.0f);
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

void Human::Jump(float strength) {
  if (IsRelaxed()) {
    gravity_velocity_ = strength;
    state_ = State::kJumping;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kJump;
    animation_looped_ = false;
  }
}

void Human::Kick() {
  if (IsRelaxed()) {
    state_ = State::kAttacking;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kKick;
    animation_looped_ = false;
    AttackEvent attack;
    attack.attacker = id_;
    attack.damage = 1.0f;
    attack.hitbox = GenWorldAabb();
    attack_queue_->push_back(attack);
  }
}

void Human::Stunned() {
  if (IsRelaxed()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kStunned;
    animation_looped_ = false;
  }
}

void Human::Fall() {
  if (state_ != State::kFalling) {
    state_ = State::kFalling;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kFall;
    animation_looped_ = true;
  }
}

void Human::ApplyGravity(UiRenderData& render_data) {
  float ground_height =
      render_data.glfw_context_.tile_renderer->cur_tile_.GetPositionY(
          position_.x * 16.0f + 512.0f, position_.z * 16.0f + 512.0f);
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

void Human::ResetState() {
  state_ = State::kIdle;
  animation_id_ = Animation::kIdle;
  animation_time_ = 0.0f;
  animation_looped_ = true;
}
