#include "PlayerHuman.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../../io/Camera.h"
#include "ModelLoader.h"

PlayerHuman::PlayerHuman(UiSharedResources& ui_shared_resources)
    : fpv_(ui_shared_resources) {
  id_ = gEntityIdManager.PlayerHumanId;
}

void PlayerHuman::Render(UiSharedResources& ui_shared_resources) {
  // std::cout << position_.y << std::endl;
  UpdateAnimation();
  // if reached time point OR animation's over (idle state)
  if (!next_event_.done &&
      (animation_time_ >= next_event_.time || state_ == State::kIdle)) {
    next_event_.done = true;
    FireEvent(next_event_.type);
  }
  ui_shared_resources.shader_animated_mdl_.Bind();
  model_data_->BindTextures();
  auto model = GenModelMat(ui_shared_resources, 1.f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
  if (state_ == State::kFpv) {
    fpv_.Render(ui_shared_resources);
  } else {
    fpv_.RenderRigged(ui_shared_resources);
  }
}

void PlayerHuman::ProcessMovement(int key, int action) {
  if (IsFpv()) {
    fpv_.ProcessMovement(key, action);
    return;
  }
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_W)
      SetMoveForward(true);
    else if (key == GLFW_KEY_S)
      SetMoveBackward(true);
    else if (key == GLFW_KEY_A)
      SetMoveLeft(true);
    else if (key == GLFW_KEY_D)
      SetMoveRight(true);
    else if (key == GLFW_KEY_SPACE)
      Jump(3.0f);
    else if (key == GLFW_KEY_EQUAL)
      Kick();
    else if (key == GLFW_KEY_MINUS)
      Stunned();
    else if (key == GLFW_KEY_T)
      Rest();
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W)
      SetMoveForward(false);
    else if (key == GLFW_KEY_S)
      SetMoveBackward(false);
    else if (key == GLFW_KEY_A)
      SetMoveLeft(false);
    else if (key == GLFW_KEY_D)
      SetMoveRight(false);
  }
}

// crouch, fall, idle, run, walk - after end loop
// jump, kick, stunned - after end keep last frame till other anim set

void PlayerHuman::Update(UiSharedResources& ui_shared_resources) {
  if (state_ == State::kFpv) {
    fpv_.Update(ui_shared_resources);
    return;
  }
  if (animation_id_ == Animation::kIdleSitting) {
    return;
  }

  if (glfwGetKey(gWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    speed_ = 1.0f;
  } else if (glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    speed_ = 0.1f;
  } else {
    speed_ = 0.25f;
  }
  acceleration_ = 0.1f;   // tweak for snappier movement
  float friction = 6.0f;  // slows down when no input

  auto camera = ui_shared_resources.glfw_context_.camera;
  glm::vec3 forward = -camera->GetDirectionWorldFront();
  glm::vec3 right = camera->GetDirectionRight();

  // --- Compute input direction ---
  glm::vec3 input_dir(0.0f);
  if (IsIdle()) {
    if (move_forward_) input_dir += forward;
    if (move_backward_) input_dir -= forward;
    if (move_right_) input_dir += right;
    if (move_left_) input_dir -= right;
  }

  // Normalize to prevent diagonal speed boost
  glm::vec3 desired_velocity(0.0f);
  float input_len = glm::length(input_dir);
  if (input_len > 0.0f) desired_velocity = input_dir / input_len * speed_;

  // --- Accelerate velocity toward desired ---
  glm::vec3 delta_v = desired_velocity - velocity_;
  float delta_len = glm::length(delta_v);
  float accel_step = acceleration_ * gDeltaTime;
  if (delta_len <= accel_step) {
    velocity_ = desired_velocity;
  } else {
    velocity_ += delta_v / delta_len * accel_step;
  }

  // --- Friction when no input ---
  if (IsIdle()) {
    if (input_len == 0.0f) {
      float vel_len = glm::length(velocity_);
      if (vel_len > 0.0f) {
        float decel = friction * gDeltaTime;
        if (decel >= vel_len) {
          velocity_ = glm::vec3(0.0f);
        } else {
          velocity_ *= (vel_len - decel) / vel_len;
        }
      }
    }
  }

  // --- Update position ---
  position_ += velocity_ * gDeltaTime;
  position_ = glm::clamp(position_, -32.0f, 32.0f);

  auto velocity_mag = glm::length(velocity_);
  bool is_moving = velocity_mag > 0.001f;

  Animation new_animationd_id;

  // --- Update rotation to follow velocity ---
  if (is_moving) {  // avoid jitter
    glm::vec3 move_dir = glm::normalize(velocity_);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right_vec = glm::normalize(glm::cross(up, move_dir));
    glm::vec3 corrected_up = glm::cross(move_dir, right_vec);
    glm::mat3 rotMat(right_vec, corrected_up, move_dir);  // columns: X,Y,Z
    rotation_ =
        glm::slerp(rotation_, glm::quat_cast(rotMat), 5.0f * gDeltaTime);
    if (velocity_mag < .35f) {
      new_animationd_id = Animation::kWalk;
    } else {
      new_animationd_id = Animation::kRun;
    }
    // new_animationd_id = HumanAnimation::kCrouch;
  } else {
    new_animationd_id = Animation::kIdle;
  }

  if (IsIdle() && animation_id_ != new_animationd_id) {
    animation_id_ = new_animationd_id;
    animation_time_ = 0.0f;
    animation_looped_ = true;
  }

  ApplyGravity(ui_shared_resources);

  fpv_.SetPosition(position_);
  fpv_.SetRotation(rotation_);
}

// should be called in Render(), it updates skin ubo
void PlayerHuman::UpdateAnimation() {
  bool started_over = animator_->UpdateUbo(animation_id_, animation_time_);
  if (started_over && !animation_looped_) {
    ResetState();
  }
}

void PlayerHuman::Jump(float strength) {
  if (IsIdle()) {
    gravity_velocity_ = strength;
    state_ = State::kJumping;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kJump;
    animation_looped_ = false;
  }
}

void PlayerHuman::Kick() {
  if (IsIdle()) {
    state_ = State::kAttacking;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kKick;
    animation_looped_ = false;
    next_event_ = Event(0.7f, PlayerEventType::DealDamage, false);
  }
}

void PlayerHuman::Stunned() {
  if (IsIdle()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kStunned;
    animation_looped_ = false;
  }
}

void PlayerHuman::Rest() {
  if (IsIdle()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kIdleSitting;
    animation_looped_ = false;
    fpv_.SetPosition(position_ + glm::vec3(0.01f, 0.0f, 0.01f));
  }
}

void PlayerHuman::Fall() {
  if (state_ != State::kFalling) {
    state_ = State::kFalling;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kFall;
    animation_looped_ = true;
  }
}

void PlayerHuman::ApplyGravity(UiSharedResources& ui_shared_resources) {
  float ground_height =
      ui_shared_resources.glfw_context_.tile_renderer->cur_tile_.GetPositionY(
          position_.x * 16.0f + 512.0f, position_.z * 16.0f + 512.0f);
  if (!IsOnGround()) {
    const float gravity = -9.81f;
    const float mass = 1.0f;
    gravity_velocity_ += gDeltaTime * gravity;
    position_.y += gravity_velocity_ * gDeltaTime * mass;
    if (position_.y <= ground_height) {
      position_.y = ground_height;
      gravity_velocity_ = 0.0f;
      bool hurt = false;
      if (state_ == State::kFalling) {
        hurt = true;
      }
      ResetState();
      if (hurt) {
        Stunned();
      }
    } else {
      if (gravity_velocity_ >= 6.0f) {
        Fall();
      }
    }
  } else {
    position_.y = ground_height;
  }
}

void PlayerHuman::ResetState() {
  animation_id_ = Animation::kIdle;
  animation_time_ = 0.0f;
  animation_looped_ = true;
  state_ = State::kIdle;
}

void PlayerHuman::SwitchToFpv() {
  if (state_ != State::kIdle) {
    return;
  }
  state_ = State::kFpv;
  animation_time_ = 0.0f;
  animation_id_ = Animation::kThrow;
  animation_looped_ = false;
  next_event_ = Event(100.0f, PlayerEventType::EnterIdleSitting, false);
  fpv_.SetPosition(position_ + glm::vec3(0.0f, 0.01f, 0.0f));
  fpv_.ResetRotation();
}

void PlayerHuman::SwitchToHuman() {
  if (state_ != State::kFpv) {
    return;
  }
  animation_time_ = 0.0f;
  animation_id_ = Animation::kThrow;
  animation_looped_ = false;
  next_event_ = Event(100.0f, PlayerEventType::EnterIdle, false);
}

void PlayerHuman::FireEvent(PlayerEventType e) {
  switch (e) {
    case PlayerEventType::DealDamage:
      DealDamage();
      break;
    case PlayerEventType::EnterIdle:
      EnterIdle();
      break;
    case PlayerEventType::EnterIdleSitting:
      EnterIdleSitting();
      break;
    default:
      break;
  }
}

void PlayerHuman::DealDamage() {
  AttackEvent attack;
  attack.attacker = id_;
  attack.damage = 1.0f;
  attack.hitbox = GenWorldAabb();
  attack_queue_->push_back(attack);
}

void PlayerHuman::EnterIdle() {
  state_ = State::kIdle;
  animation_time_ = 0.0f;
  animation_id_ = Animation::kIdle;
  animation_looped_ = true;
}

void PlayerHuman::EnterIdleSitting() {
  state_ = State::kFpv;
  animation_time_ = 0.0f;
  animation_id_ = Animation::kIdleSitting;
  animation_looped_ = true;
}
