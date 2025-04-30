#include "Cameras.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../common/Details.h"
#include "Window.h" // for frame rate (gDeltaTime)
#include "../common/OpenGlUtility.h"
#include "../common/ShadersBinding.h"

ICamera::ICamera(GLuint ubo)
    : ubo_(ubo),
      speed_(1.0f),
      yaw_(-90.0f),
      pitch_(0.0f),
      move_vector_(),
      position_{0.0f, 0.0f, 0.0f},
      direction_front_{0.0f, 0.0f, -1.0f},
      direction_up_{0.0f, 1.0f, 0.0f},
      direction_right_{0.0f},
      direction_world_up_{direction_up_} {
  UpdateConfig();
}

//position
void ICamera::SetMoveForward(float magnitude) {
  move_vector_.y = magnitude;
}
void ICamera::SetMoveBackward(float magnitude) {
  move_vector_.y = -magnitude;
}
void ICamera::SetMoveLeft(float magnitude) {
  move_vector_.x = -magnitude;
}
void ICamera::SetMoveRight(float magnitude) {
  move_vector_.x = magnitude;
}

//direction TODO: time-based?
void ICamera::SetRotateOnYaw(float angle, float delay) {
  // TODO: delay + blur(?)
  //    yaw_add_ = angle; // is it event?
}

void ICamera::SetRotateOnPitch(float angle, float delay) {
  //
}
//TODO; but... is it bind to player character... I mean camera always
// should lag behind the player and then recover after player stop...
// Need to think more about it....

//combinated
void ICamera::SmoothRotation(glm::vec3 target_pos, float yaw,
                             float pitch, float seconds) {

}

void ICamera::UpdateFrame() {
  MoveOnX();
  MoveOnZ();
  UpdateViewMatrix();
}

void ICamera::UpdateConfig() {
  UpdateCameraVectors();
  UpdateViewMatrix();
  UpdateProjectionMatrix();
}

void ICamera::UpdateViewMatrix() const {
  glm::mat4 view_mat = glm::lookAt(position_, position_ + direction_front_,
                                   direction_up_);
  utility::UpdateUbo(ubo_, 0, 64, glm::value_ptr(view_mat));
}

//TODO: should be called from WindowSizeCallback
void ICamera::UpdateProjectionMatrix() const {
  glm::mat4 proj_mat = glm::perspective(
      glm::radians(45.0f), static_cast<float>(details::kWindowWidth)
                               / static_cast<float>(details::kWindowHeight),
      0.01f, 1000.0f);
  utility::UpdateUbo(ubo_, 64, 64, glm::value_ptr(proj_mat));
}

void ICamera::UpdateCameraVectors() {
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

void ICamera::MoveOnX() {
  position_ += direction_right_ * speed_ * gDeltaTime * move_vector_.x;
}
void ICamera::MoveOnZ() {
  position_ += direction_front_ * speed_ * gDeltaTime * move_vector_.y;
}

void GameCamera::ProcessMouseMovement(float xoffset, float yoffset) {
  /* highlight the area under the cursor - done with some unknown magic */

  yaw_ += xoffset * sensitivity_;
  pitch_ += yoffset * sensitivity_;

  /// constrained pitch
  pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
  UpdateCameraVectors();
}

void InspectCamera::ProcessMouseMovement(float xoffset, float yoffset) {
  yaw_ += xoffset * sensitivity_;
  pitch_ += yoffset * sensitivity_;

  /// constrained pitch
  pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
  UpdateCameraVectors();
}

MapCamera::MapCamera(GLuint ubo)
    : ICamera(ubo) {
  position_.y = 10.0f;
  pitch_ = -89.9f;
  yaw_ = 0.0f;
}

#include <iostream>

void MapCamera::ProcessMouseMovement(float xoffset, float yoffset) {
  std::cout << xoffset << " and " << yoffset << std::endl;
  position_.x += yoffset * speed_ / 10.0f;
  position_.z += xoffset * speed_ / 10.0f;
  position_.x = std::clamp(position_.x, -4.0f, 4.0f);
  position_.z = std::clamp(position_.z, -4.0f, 4.0f);
  std::cout << "position: " << position_.x << ' '
            << position_.y << ' ' << position_.z << std::endl;
  UpdateCameraVectors();
}

CameraHandler::CameraHandler() {
  InitUbo();
  cameras_[0] = std::make_unique<InspectCamera>(ubo_);
  cameras_[1] = std::make_unique<GameCamera>(ubo_);
  cameras_[2] = std::make_unique<MapCamera>(ubo_);
  SetInspectCamera();
  // disable cursor
  do_show_cursor_ = true;
  ToggleCursor();
}

CameraHandler::~CameraHandler() {
  DeInitUbo();
}

void CameraHandler::ToggleCursor() {
  if (do_show_cursor_) {
    cursor_.SetModeEnabled();
  } else {
    cursor_.SetModeDisabled();
  }
  do_show_cursor_ = !do_show_cursor_;
}

void CameraHandler::Update() {
  cameras_[cur_id_]->UpdateFrame();
}

void CameraHandler::ProcessMouseMovement(float xoffset, float yoffset) {
  cameras_[cur_id_]->ProcessMouseMovement(xoffset, yoffset);
}

void CameraHandler::ProcessMouseKey(int button, int action, int mods) {
  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_MIDDLE) {
    ToggleCursor();
  }
  cameras_[cur_id_]->ProcessMouseKey(button, action, mods);
}

void CameraHandler::ProcessMouseScroll(float yoffset) {
  cameras_[cur_id_]->ProcessMouseScroll(yoffset);
}

void CameraHandler::ProcessKey(int key, int scancode, int action, int mods) {
  cameras_[cur_id_]->ProcessKey(key, scancode, action, mods);
}

//TODO: need also to handle other camera effects... like rotation for GameCamera
// maybe this..:
ICamera* CameraHandler::GetCurCamera() {
  return cameras_[cur_id_].get();
}

//game -> editor -> map -> menu
void CameraHandler::SetInspectCamera() {
  SetCamera(0);
}

void CameraHandler::SetGameCamera() {
  SetCamera(1);
}

void CameraHandler::SetMapCamera() {
  SetCamera(2);
}

void CameraHandler::InitUbo() {
  glGenBuffers(1, &ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, shader::kUboCameraBind, ubo_);
}

void CameraHandler::DeInitUbo() {
  glDeleteBuffers(GL_UNIFORM_BUFFER, &ubo_);
}

void CameraHandler::SetCamera(int id) {
  cur_id_ = id;
  auto camera = cameras_[cur_id_].get();
  camera->UpdateConfig();
}
