#include "Camera.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/Details.h"
#include "Window.h"  // for frame rate (gDeltaTime)

void Camera::ProcessMovement(int key, int action) {
  std::cerr << "not that ProvcessMovement<, need plauer" << std::endl;
}

glm::vec3 ToGlm(JPH::Vec3 val) {
  return glm::vec3(val.GetX(), val.GetY(), val.GetZ());
}
JPH::Vec3 ToJph(glm::vec3 val) {
  return JPH::Vec3(val.x, val.y, val.z);
}

JPH::Quat ToJph(glm::quat val) {
  return JPH::Quat(val.x, val.y, val.z, val.w).Normalized();
}

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

Camera::Camera()
    : speed_(1.0f),
      yaw_(-90.0f),
      pitch_(0.0f),
      position_{5.0f, 5.0f, 5.0f},
      direction_front_{0.0f, 0.0f, -1.0f},
      direction_up_{0.0f, 1.0f, 0.0f},
      direction_right_{0.0f},
      direction_world_up_{0.0f, 1.0f, 0.0f} {
  Init();
}

void Camera::Init() {
  glCreateBuffers(1, &ubo_);
  glNamedBufferStorage(ubo_, sizeof(CameraUBO), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_);
  MoveRotateView(0.0f, 0.0f);
  Update(JPH::Mat44::sIdentity());
}

void Camera::DeInit() { glDeleteBuffers(1, &ubo_); }

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

void Camera::ZoomOriginDist(float yoffset) {
  origin_dist_ = glm::clamp(origin_dist_ - yoffset, 5.0f, 15.0f);
}

void Camera::Update(JPH::Mat44 head_mat) {
  auto head_pos = ToGlm(head_mat.GetTranslation());
  // head_pos.y += 4.0f;
  if (first_face_mode_) {
    position_ = head_pos;
    origin_ = head_pos;
    direction_front_.x = glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
    direction_front_.y = glm::sin(glm::radians(pitch_));
    direction_front_.z = glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
    direction_front_ = glm::normalize(direction_front_);
    direction_right_ = glm::normalize(glm::cross(direction_front_, direction_world_up_));
    direction_up_    = glm::normalize(glm::cross(direction_right_, direction_front_));
  } else {
    origin_ = head_pos;
    float yawRad = glm::radians(yaw_);
    float pitchRad = glm::radians(pitch_);
    // float origin_dist = glm::length(origin_ - position_);
    position_.x = origin_.x + origin_dist_ * cos(pitchRad) * sin(yawRad);
    position_.y = origin_.y + origin_dist_ * sin(pitchRad);
    position_.z = origin_.z + origin_dist_ * cos(pitchRad) * cos(yawRad);
    direction_front_ = glm::normalize(origin_ - position_);
    direction_right_ =
        glm::normalize(glm::cross(direction_front_, direction_world_up_));
    direction_up_ = glm::cross(direction_right_, direction_front_);
    direction_world_front_ = glm::cross(direction_right_, direction_world_up_);
  }

  CameraUBO data{};
  data.camPos = position_;
  data.camForward = direction_front_;
  data.camUp = direction_up_;
  data.camRight = direction_right_;
  data.cosHalfFov = std::cos(fovy_ * 0.5f);
  data.view =
      glm::lookAt(position_, position_ + direction_front_, direction_up_);
  data.proj = glm::perspective(
      fovy_, float(gWindowWidth) / float(gWindowHeight), 0.01f, 1000.0f);
  glNamedBufferSubData(ubo_, 0, sizeof(CameraUBO), &data);
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

// map-like
void Camera::MovePanView(float xoffset, float yoffset) {
  return;
  float sensitivity = speed_ * gDeltaTime;

  position_ -= direction_right_ * sensitivity * xoffset;
  position_ -= direction_up_ * sensitivity * yoffset;

  origin_ -= direction_right_ * sensitivity * xoffset;
  origin_ -= direction_up_ * sensitivity * yoffset;
}

void Camera::MoveRotateView(float xoffset, float yoffset) {
  if (first_face_mode_) {
    float sensitivity = speed_ * gDeltaTime;
    yaw_ += xoffset * sensitivity;
    yaw_ = std::remainder(yaw_, 360.0f);
    pitch_ += yoffset * sensitivity;
    pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
  } else {
    float sensitivity = speed_ * gDeltaTime;
    yaw_ -= xoffset * sensitivity;
    yaw_ = std::remainder(yaw_, 360.0f);
    pitch_ -= yoffset * sensitivity;
    pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);

    bool snap = (glfwGetKey(gWindow, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) ||
                (glfwGetKey(gWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);
    if (snap) {
      SnapYaw();
      SnapPitch();
    }
  }
}

void Camera::Reset() {
  SetPosition(glm::vec3{0.0f});
  SetPitch(45.0f);
  SetYaw(0.0f);
  SetOrigin(glm::vec3{0.0f});
}

void Camera::HideCursor() {
  glfwSetCursorPosCallback(gWindow, nullptr);
  glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Camera::ShowCursor() {
  glfwSetCursorPosCallback(gWindow, nullptr);
  glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Frustum Camera::GetFrustum() const noexcept {
  auto pos = GetOrigin();
  auto fwd = GetDirectionFront();
  auto up = GetDirectionUp();
  JPH::Vec3 cam_position(pos.x, pos.y, pos.z);
  JPH::Vec3 cam_inForward(fwd.x, fwd.y, fwd.z);
  JPH::Vec3 cam_inUp(up.x, up.y, up.z);
  float cam_fovx = glm::radians(120.0f);
  float cam_fovy = glm::radians(75.0f);
  float cam_inNear = 0.0f;
  return Frustum(cam_position, cam_inForward, cam_inUp,
    cam_fovx, cam_fovy, cam_inNear);
}

glm::quat Camera::GetRotation() const {
  glm::mat3 rot(direction_right_, direction_up_, -direction_front_);
  return glm::quat_cast(rot);
}
