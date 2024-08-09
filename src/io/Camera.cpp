#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../common/Details.h"
#include "Window.h" // for frame rate (gDeltaTime)

void Camera::Update() {
  for (const auto& move : movement_queue_) {
    move();
  }
  UpdateViewMatrix();
}

void Camera::UpdateViewMatrix() const {
  glm::mat4 view_mat = glm::lookAt(position_, position_ + direction_front_,
                                   direction_up_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(view_mat));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::UpdateProjectionMatrix() const {
  glm::mat4 proj_mat = glm::perspective(
      glm::radians(45.0f), static_cast<float>(details::kWindowWidth)
                               / static_cast<float>(details::kWindowHeight),
      0.01f, 1000.0f);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(proj_mat));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::SetMoveForward() {
  if (movement_queue_idx_.forward == -1) {
    movement_queue_idx_.forward = static_cast<int>(
        movement_queue_.PushBack([this] { this->MoveForward(); }));
  }
}
void Camera::SetMoveBackward() {
  if (movement_queue_idx_.backward == -1) {
    movement_queue_idx_.backward = static_cast<int>(
        movement_queue_.PushBack([this] { this->MoveBackward(); }));
  }
}
void Camera::SetMoveLeft() {
  if (movement_queue_idx_.left == -1) {
    movement_queue_idx_.left = static_cast<int>(
        movement_queue_.PushBack([this] { this->MoveLeft(); }));
  }
}
void Camera::SetMoveRight() {
  if (movement_queue_idx_.right == -1) {
    movement_queue_idx_.right = static_cast<int>(
        movement_queue_.PushBack([this] { this->MoveRight(); }));
  }
}

void Camera::UnSetMoveForward() {
  movement_queue_.Erase(movement_queue_idx_.forward);
  if (movement_queue_idx_.backward > movement_queue_idx_.forward) {
    --movement_queue_idx_.backward;
  }
  if (movement_queue_idx_.left > movement_queue_idx_.forward) {
    --movement_queue_idx_.left;
  }
  if (movement_queue_idx_.right > movement_queue_idx_.forward) {
    --movement_queue_idx_.right;
  }
  movement_queue_idx_.forward = -1;
}
void Camera::UnSetMoveBackward() {
  movement_queue_.Erase(movement_queue_idx_.backward);
  if (movement_queue_idx_.left > movement_queue_idx_.backward) {
    --movement_queue_idx_.left;
  }
  if (movement_queue_idx_.right > movement_queue_idx_.backward) {
    --movement_queue_idx_.right;
  }
  if (movement_queue_idx_.forward > movement_queue_idx_.backward) {
    --movement_queue_idx_.forward;
  }
  movement_queue_idx_.backward = -1;
}
void Camera::UnSetMoveLeft() {
  movement_queue_.Erase(movement_queue_idx_.left);
  if (movement_queue_idx_.backward > movement_queue_idx_.left) {
    --movement_queue_idx_.backward;
  }
  if (movement_queue_idx_.right > movement_queue_idx_.left) {
    --movement_queue_idx_.right;
  }
  if (movement_queue_idx_.forward > movement_queue_idx_.left) {
    --movement_queue_idx_.forward;
  }
  movement_queue_idx_.left = -1;
}
void Camera::UnSetMoveRight() {
  movement_queue_.Erase(movement_queue_idx_.right);
  if (movement_queue_idx_.backward > movement_queue_idx_.right) {
    --movement_queue_idx_.backward;
  }
  if (movement_queue_idx_.left > movement_queue_idx_.right) {
    --movement_queue_idx_.left;
  }
  if (movement_queue_idx_.forward > movement_queue_idx_.right) {
    --movement_queue_idx_.forward;
  }
  movement_queue_idx_.right = -1;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
  xoffset *= sensitivity_;
  yoffset *= sensitivity_;

  yaw_ += xoffset;
  pitch_ += yoffset;

  /// constrain pitch
  if (pitch_ > 89.0f) {
    pitch_ = 89.0f;
  } else if (pitch_ < -89.0f) {
    pitch_ = -89.0f;
  }

  UpdateCameraVectors();
}

void Camera::InitUbo() {
  glGenBuffers(1, &ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, details::kUboCameraBind, ubo_);
}

void Camera::DeInitUbo() {
  glDeleteBuffers(GL_UNIFORM_BUFFER, &ubo_);
}

void Camera::UpdateCameraVectors() {
  glm::vec3 front;
  front.x = glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
  front.y = glm::sin(glm::radians(pitch_));
  front.z = glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
  direction_front_ = glm::normalize(front);
  direction_right_ = glm::normalize(
      glm::cross(direction_front_, direction_world_up_));
  direction_up_ = glm::normalize(
      glm::cross(direction_right_, direction_front_));
}

void Camera::MoveLeft() {
  position_ -= direction_right_ * speed_ * gDeltaTime;
}

void Camera::MoveRight() {
  position_ += direction_right_ * speed_ * gDeltaTime;
}

void Camera::MoveForward() {
  position_ += direction_front_ * speed_ * gDeltaTime;
}

void Camera::MoveBackward() {
  position_ -= direction_front_ * speed_ * gDeltaTime;
}
