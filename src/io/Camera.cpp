#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../common/Details.h"
#include "../common/OpenGlUtility.h"
#include "Window.h"  // for frame rate (gDeltaTime)

struct CameraUBO {
  glm::vec3 camPos;
  float _pad0;
  glm::vec3 camForward;
  float _pad1;
  glm::vec3 camRight;
  float _pad2;
  glm::vec3 camUp;
  float cosHalfFov;
  glm::mat4 view;
  glm::mat4 proj;
};

// map scale to shaders? or just get from transform mat

Camera::Camera()
    : speed_(1.0f),
      yaw_(-90.0f),
      pitch_(0.0f),
      position_{5.0f, 5.0f, 5.0f},
      direction_front_{0.0f, 0.0f, -1.0f},
      direction_up_{0.0f, 1.0f, 0.0f},
      direction_right_{0.0f},
      direction_world_up_{direction_up_} {
  Init();
}

/*void Camera::Init() {
  InitUbo();
  UpdateCameraVectors();
  Update();
}*/

void Camera::InitUbo() {
  glCreateBuffers(1, &ubo_);
  glNamedBufferStorage(ubo_, sizeof(CameraUBO), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_);
}

void Camera::DeInitUbo() { glDeleteBuffers(1, &ubo_); }

glm::mat4 Camera::GetViewMatrix(float map_scale) const noexcept {
  auto scaled_position = map_scale * position_;
  return glm::lookAt(scaled_position, scaled_position + direction_front_,
                     direction_up_);
}

glm::mat4 Camera::GetProjMatrix() const noexcept {
  return glm::perspective(
      fovy_,
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight),
      0.01f, 1000.0f);
}

void Camera::Init() {
  InitUbo();
  UpdateCameraVectors();
  Update();
}

void Camera::Update() {
  CameraUBO data{};

  // TODO: not "position_", but world_pos, so map_scale'd

  data.camPos = position_;
  data.camForward = glm::normalize(direction_front_);
  data.camUp = glm::normalize(direction_up_);
  data.camRight = glm::normalize(direction_right_);

  data.cosHalfFov = std::cos(fovy_ * 0.5f);

  data.view =
      glm::lookAt(position_, position_ + direction_front_, direction_up_);

  data.proj = glm::perspective(
      fovy_, float(gWindowWidth) / float(gWindowHeight), 0.01f, 1000.0f);
  glNamedBufferSubData(ubo_, 0, sizeof(CameraUBO), &data);
}

void Camera::UpdateViewMatrix() const {
  glm::mat4 view_mat =
      glm::lookAt(position_, position_ + direction_front_, direction_up_);
  utility::UpdateUbo(ubo_, 0, 64, glm::value_ptr(view_mat));
}

void Camera::UpdateProjectionMatrix() const {
  glm::mat4 proj_mat = glm::perspective(
      glm::radians(45.0f),
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight),
      0.01f, 1000.0f);
  utility::UpdateUbo(ubo_, 64, 64, glm::value_ptr(proj_mat));
}

void Camera::UpdateUboPos() const {
  utility::UpdateUbo(ubo_, 128, 12, glm::value_ptr(origin_));
}

void Camera::UpdateCameraVectors(float radius) {
  glm::vec3 front;
  front.x = glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
  front.y = glm::sin(glm::radians(pitch_));
  front.z = glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
  front *= radius;
  direction_front_ = glm::normalize(-front);
  direction_right_ =
      glm::normalize(glm::cross(direction_front_, direction_world_up_));
  direction_up_ =
      glm::normalize(glm::cross(direction_right_, direction_front_));
  direction_world_front_ = glm::cross(direction_right_, direction_world_up_);
}

void Camera::MovePanView(float xoffset, float yoffset) {
  float sensitivity = speed_ * gDeltaTime;

  position_ -= direction_right_ * sensitivity * xoffset;
  position_ -= direction_up_ * sensitivity * yoffset;

  origin_ -= direction_right_ * sensitivity * xoffset;
  origin_ -= direction_up_ * sensitivity * yoffset;
}

void Camera::SnapYaw() {
  yaw_ = std::fmod(yaw_, 360.0f);
  if (yaw_ < 0.0f) {
    yaw_ += 360.0f;
  }
  yaw_ = std::round(yaw_ / 90.0f) * 90.0f;
}

void Camera::SnapPitch() {
  if (pitch_ < -45.0f) {
    pitch_ = -89.9f;
  } else if (pitch_ > 45.0f) {
    pitch_ = 89.9f;
  } else {
    pitch_ = 0.0f;
  }
}

void Camera::MoveRotateViewOrigin(float xoffset, float yoffset) {
  float sensitivity = speed_ * gDeltaTime;
  yaw_ -= xoffset * sensitivity;
  pitch_ -= yoffset * sensitivity;
  pitch_ = std::clamp(pitch_, -89.0f, 89.0f);

  bool snap = (glfwGetKey(gWindow, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) ||
              (glfwGetKey(gWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);
  if (snap) {
    SnapYaw();
    SnapPitch();
  }

  float yawRad = glm::radians(yaw_);
  float pitchRad = glm::radians(pitch_);

  float origin_dist = glm::length(origin_ - position_);

  position_.x = origin_.x + origin_dist * cos(pitchRad) * sin(yawRad);
  position_.y = origin_.y + origin_dist * sin(pitchRad);
  position_.z = origin_.z + origin_dist * cos(pitchRad) * cos(yawRad);
  direction_front_ = glm::normalize(origin_ - position_);
  direction_right_ =
      glm::normalize(glm::cross(direction_front_, direction_world_up_));
  direction_up_ = glm::cross(direction_right_, direction_front_);
  direction_world_front_ = glm::cross(direction_right_, direction_world_up_);
}

void Camera::MoveRotateViewOriginDist(float xoffset) {
  float sensitivity = speed_ * gDeltaTime;
  yaw_ -= xoffset * sensitivity;

  bool snap = (glfwGetKey(gWindow, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) ||
              (glfwGetKey(gWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);
  if (snap) {
    SnapYaw();
  }

  float yawRad = glm::radians(yaw_);
  float pitchRad = glm::radians(pitch_);

  position_.x = origin_.x + origin_dist_ * cos(pitchRad) * sin(yawRad);
  position_.y = origin_.y + origin_dist_ * sin(pitchRad);
  position_.z = origin_.z + origin_dist_ * cos(pitchRad) * cos(yawRad);
  direction_front_ = glm::normalize(origin_ - position_);
  direction_right_ =
      glm::normalize(glm::cross(direction_front_, direction_world_up_));
  direction_up_ = glm::cross(direction_right_, direction_front_);
  direction_world_front_ = glm::cross(direction_right_, direction_world_up_);
}
