#include "PlayerHuman.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../../io/Camera.h"

PlayerHuman::PlayerHuman(UiSharedResources& ui_shared_resources)
    : PlayerBase(ui_shared_resources) {
  id_ = gEntityIdManager.PlayerHumanId;
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
  acceleration_ = 0.1f;   // tweak for snappier movement
  float friction = 2.0f;  // slows down when no input

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
  HumanAnimator::Type new_animationd_id;

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
      new_animationd_id = HumanAnimator::Type::kRun;
      speed_ = 1.0f;
    } else if (glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      new_state = State::kCrouching;
      new_animationd_id = HumanAnimator::Type::kCrouch;
      speed_ = 0.1f;
    } else {
      new_state = State::kWalking;
      new_animationd_id = HumanAnimator::Type::kWalk;
      speed_ = 0.25f;
    }
  } else {
    new_state = State::kIdle;
    new_animationd_id = HumanAnimator::Type::kIdle;
  }

  if (IsRelaxed() && new_state != state_) {
    ResetState();
    state_ = new_state;
    animation_id_ = new_animationd_id;
    animation_time_ = 0.0f;
    animation_looped_ = true;
  }

  ApplyGravity(ui_shared_resources);
}
