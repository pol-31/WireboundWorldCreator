#include "PlayerHuman.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../../io/Camera.h"
#include "ModelLoader.h"

PlayerHuman::PlayerHuman(UiSharedResources& ui_shared_resources, PlayerFpv& fpv)
    : PlayerBase(ui_shared_resources), fpv_(fpv) {
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
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

void PlayerHuman::ProcessMovement(int key, int action) {
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
      Jump(5.0f);
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
  if (state_ == State::kOnFpv) {
    return;
  }
  acceleration_ = 0.1f;   // tweak for snappier movement
  float friction = 6.0f;  // slows down when no input

  auto camera = ui_shared_resources_.glfw_context_.camera;
  glm::vec3 forward = -camera->GetDirectionWorldFront();
  glm::vec3 right = camera->GetDirectionRight();

  // --- Compute input direction ---
  glm::vec3 input_dir(0.0f);
  if (IsRelaxed()) {
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
  if (delta_len <= accel_step)
    velocity_ = desired_velocity;
  else
    velocity_ += delta_v / delta_len * accel_step;

  // --- Friction when no input ---
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

  // --- Update position ---
  position_ += velocity_ * gDeltaTime;
  position_ = glm::clamp(position_, -32.0f, 32.0f);

  auto velocity_mag = glm::length(velocity_);
  bool is_moving = velocity_mag > 0.001f;

  State new_state;
  HumanAnimation new_animationd_id;

  // --- Update rotation to follow velocity ---
  if (is_moving) {  // avoid jitter
    glm::vec3 move_dir = glm::normalize(velocity_);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right_vec = glm::normalize(glm::cross(up, move_dir));
    glm::vec3 corrected_up = glm::cross(move_dir, right_vec);
    glm::mat3 rotMat(right_vec, corrected_up, move_dir);  // columns: X,Y,Z
    rotation_ =
        glm::slerp(rotation_, glm::quat_cast(rotMat), 5.0f * gDeltaTime);
    if (glfwGetKey(gWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      new_state = State::kRunning;
      new_animationd_id = HumanAnimation::kRun;
      speed_ = 1.0f;
    } else if (glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      new_state = State::kCrouching;
      new_animationd_id = HumanAnimation::kCrouch;
      speed_ = 0.1f;
    } else {
      new_state = State::kWalking;
      new_animationd_id = HumanAnimation::kWalk;
      speed_ = 0.25f;
    }
  } else {
    new_state = State::kIdle;
    new_animationd_id = HumanAnimation::kIdle;
  }

  if (IsRelaxed() && new_state != state_) {
    ResetState();
    state_ = new_state;
    animation_id_ = new_animationd_id;
    animation_time_ = 0.0f;
    animation_looped_ = true;
    fpv_.SetAnimation(static_cast<FpvAnimation>(animation_id_), animation_time_,
                      animation_looped_);
  }

  ApplyGravity(ui_shared_resources);

  fpv_.SetPosition(position_);
  fpv_.SetRotation(rotation_);
}

void PlayerHuman::RenderPicking(UiSharedResources& ui_shared_resources) {
  // TODO: render capsule model instead
  UpdateAnimation();
  ui_shared_resources.shader_animated_mdl_picking_.Bind();
  glUniform1ui(1, static_cast<uint32_t>(details::kIdOffsetObjects + id_));
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

// should be called in Render(), it updates skin ubo
void PlayerHuman::UpdateAnimation() {
  bool started_over = animator_->UpdateUbo(animation_id_, animation_time_);
  if (started_over && !animation_looped_) {
    ResetState();
  }
}

void PlayerHuman::UpdatePositionY(UiSharedResources& ui_shared_resources) {
  float ground_height =
      ui_shared_resources.glfw_context_.tile_renderer->cur_tile_.GetPositionY(
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

void PlayerHuman::Jump(float strength) {
  if (IsRelaxed()) {
    gravity_velocity_ = strength;
    state_ = State::kJumping;
    animation_time_ = 0.0f;
    animation_id_ = HumanAnimation::kJump;
    animation_looped_ = false;
    fpv_.SetAnimation(static_cast<FpvAnimation>(animation_id_), animation_time_,
                      animation_looped_);
  }
}

void PlayerHuman::Kick() {
  if (IsRelaxed()) {
    state_ = State::kAttacking;
    animation_time_ = 0.0f;
    animation_id_ = HumanAnimation::kKick;
    animation_looped_ = false;
    next_event_ = Event(0.7f, PlayerEventType::DealDamage, false);
    fpv_.SetAnimation(static_cast<FpvAnimation>(animation_id_), animation_time_,
                      animation_looped_);
  }
}

void PlayerHuman::Stunned() {
  if (IsRelaxed()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = HumanAnimation::kStunned;
    animation_looped_ = false;
    fpv_.SetAnimation(static_cast<FpvAnimation>(animation_id_), animation_time_,
                      animation_looped_);
  }
}

void PlayerHuman::Rest() {
  if (IsRelaxed()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = HumanAnimation::kIdleSitting;
    animation_looped_ = false;
    fpv_.SetAnimation(FpvAnimation::kIdleSitting, animation_time_, true);
  }
}

void PlayerHuman::Fall() {
  if (state_ != State::kFalling) {
    state_ = State::kFalling;
    animation_time_ = 0.0f;
    animation_id_ = HumanAnimation::kFall;
    animation_looped_ = true;
    fpv_.SetAnimation(static_cast<FpvAnimation>(animation_id_), animation_time_,
                      animation_looped_);
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

void PlayerHuman::ResetState() {
  animation_id_ = HumanAnimation::kIdle;
  animation_time_ = 0.0f;
  animation_looped_ = true;
  state_ = State::kIdle;
  fpv_.SetAnimation(static_cast<FpvAnimation>(animation_id_), animation_time_,
                    animation_looped_);
  // if (state_ != State::kOnFpv) {
  //   state_ = State::kIdle;
  //   fpv_.SetAnimation(static_cast<FpvAnimation>(animation_id_),
  //   animation_time_,
  //                     animation_looped_);
  //}
}

void PlayerHuman::SwitchToFpv() {
  state_ = State::kOnFpv;
  animation_time_ = 0.0f;
  animation_id_ = HumanAnimation::kThrow;
  animation_looped_ = false;
  next_event_ = Event(100.0f, PlayerEventType::EnterIdleSitting, false);
  fpv_.SetPosition(position_ + glm::vec3(0.0f, 0.01f, 0.0f));
  fpv_.ResetRotation();
}

void PlayerHuman::SwitchToHuman() {
  animation_time_ = 0.0f;
  animation_id_ = HumanAnimation::kThrow;
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
  animation_id_ = HumanAnimation::kIdle;
  animation_looped_ = true;
}

void PlayerHuman::EnterIdleSitting() {
  state_ = State::kOnFpv;
  animation_time_ = 0.0f;
  animation_id_ = HumanAnimation::kIdleSitting;
  animation_looped_ = true;
}
