#include "PlayerFpv.h"

#include "../../core/TileRenderer.h"
#include "../../io/Camera.h"

PlayerFpv::PlayerFpv(UiSharedResources& ui_shared_resources)
    : PlayerBase(ui_shared_resources) {
  id_ = gEntityIdManager.PlayerFpvId;
}

void PlayerFpv::ProcessMovement(int key, int action) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_W)
      SetMoveForward(true);
    else if (key == GLFW_KEY_S)
      SetMoveBackward(true);
    else if (key == GLFW_KEY_A)
      SetMoveLeft(true);
    else if (key == GLFW_KEY_D)
      SetMoveRight(true);
    else if (key == GLFW_KEY_Q)
      SetRotateCcw(true);
    else if (key == GLFW_KEY_E)
      SetRotateCw(true);
    else if (key == GLFW_KEY_SPACE)
      speed_ = 5.0f;  // TODO: mod thrust instead
    else if (key == GLFW_KEY_LEFT_CONTROL)
      speed_ = 1.0f;  // TODO: mod thrust instead
    // TODO: while LEFT_SHIFT increases rotation speed
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W)
      SetMoveForward(false);
    else if (key == GLFW_KEY_S)
      SetMoveBackward(false);
    else if (key == GLFW_KEY_A)
      SetMoveLeft(false);
    else if (key == GLFW_KEY_D)
      SetMoveRight(false);
    else if (key == GLFW_KEY_Q)
      SetRotateCcw(false);
    else if (key == GLFW_KEY_E)
      SetRotateCw(false);
  }
}

void PlayerFpv::Update(UiSharedResources& ui_shared_resources) {
  animator_->UpdateUbo(animation_id_, animation_time_);

  /// update rotation
  float angle_speed = glm::radians(30.0f) * gDeltaTime;

  if (rotate_ccw_) yaw_ += angle_speed;
  if (rotate_cw_) yaw_ -= angle_speed;

  if (move_forward_) pitch_ -= angle_speed;
  if (move_backward_) pitch_ += angle_speed;
  if (move_left_) roll_ += angle_speed;
  if (move_right_) roll_ -= angle_speed;

  glm::vec3 world_up(0.0f, 1.0f, 0.0f);

  // --- build quaternions ---
  glm::quat yaw_q = glm::angleAxis(yaw_, world_up);

  // yaw frame axes
  glm::vec3 yaw_forward = yaw_q * glm::vec3(0, 0, -1);
  glm::vec3 yaw_right = glm::normalize(glm::cross(world_up, yaw_forward));

  // pitch/roll
  glm::quat pitch_q = glm::angleAxis(pitch_, yaw_right);
  glm::quat roll_q = glm::angleAxis(roll_, yaw_forward);

  // --- final rotation ---
  rotation_ = roll_q * pitch_q * yaw_q;

  // --- thrust axes in world space ---
  glm::vec3 forward = rotation_ * glm::vec3(0, 0, -1);
  glm::vec3 right = rotation_ * glm::vec3(1, 0, 0);
  glm::vec3 up = rotation_ * glm::vec3(0, 1, 0);

  /// now update the position
  acceleration_ = 2.0f;
  glm::vec3 thrust(0.0f);
  thrust += forward * std::sin(pitch_ * 2.0f) / 100.0f;
  thrust += right * std::sin(roll_ * 2.0f) / 100.0f;

  velocity_ += thrust;
  if (glm::length(velocity_) > speed_) {
    velocity_ = glm::normalize(velocity_) * speed_;
  }

  position_ += velocity_ * gDeltaTime;
  position_ = glm::clamp(position_, -32.0f, 32.0f);

  /*// Normalize to prevent diagonal speed boost


  // --- Friction when no input ---
  if (input_len == 0.0f) {
    float vel_len = glm::length(velocity_);
    if (vel_len > 0.0f) {
      float decel = friction * gDeltaTime;
      if (decel >= vel_len)
        velocity_ = glm::vec3(0.0f);
      else
        velocity_ *= (vel_len - decel) / vel_len;
    }
  }*/

  // --- Update position ---

  UpdatePositionY(ui_shared_resources);
}
