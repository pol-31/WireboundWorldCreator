#ifndef WIREBOUNDWORLDCREATOR_SRC_IO_CAMERAS_H_
#define WIREBOUNDWORLDCREATOR_SRC_IO_CAMERAS_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

// edit / game mode (menu not included)
class ICamera {
 public:
  //TODO: ubo shared between two cameras, so should be created externaly
  ICamera(GLuint camera_ubo_id_,
             const float& player_speed /*for editor player-stub*/)
      : speed_(player_speed) {
    UpdateCameraVectors();
    UpdateViewMatrix();
    UpdateProjectionMatrix();
  }


  /// camera move:
  /// left x-
  /// right x+
  /// forward y+
  /// back y-

  //position
  void SetMoveForward(float magnitude = 1.0f) {
    move_vector_.y = magnitude;
  }
  void SetMoveBackward(float magnitude = 1.0f) {
    move_vector_.y = -magnitude;
  }
  void SetMoveLeft(float magnitude = 1.0f) {
    move_vector_.x = -magnitude;
  }
  void SetMoveRight(float magnitude = 1.0f) {
    move_vector_.x = magnitude;
  }

  //direction TODO: time-based?
  void SetRotateOnYaw(float angle, float seconds) {
    //
  }
  void SetRotateOnPitch(float angle, float seconds) {
    //
  }
  //TODO; but... is it bind to player character... I mean camera always
  // should lag behind the player and then recover after player stop...
  // Need to think more about it....

  //combinated
  void SmoothRotation(glm::vec3 target_pos, float yaw, float pitch,
                      float seconds) {
    //
  }

  /// camera lookup
  virtual void ProcessMouseMovement(float xoffset, float yoffset) = 0;


  //To smoothly rotate related to object we need set yaw and set the position,
  // then just Update()

  //TODO: more setters:
  void SetYaw(float yaw) {
    yaw_ = yaw;
  }
  void SetPitch(float pitch) {
    pitch_ = pitch;
  }
  void SetPosition(glm::vec3 position) {
    position_ = position;
  }

  void Update() {
    MoveOnX();
    MoveOnZ();
    UpdateViewMatrix();
  }

  void UpdateViewMatrix() const {
    glm::mat4 view_mat = glm::lookAt(position_, position_ + direction_front_,
                                     direction_up_);
    utility::UpdateUbo(ubo_, 0, 64, glm::value_ptr(view_mat));
  }

  //TODO: should be called from WindowSizeCallback
  void UpdateProjectionMatrix() const {
    glm::mat4 proj_mat = glm::perspective(
        glm::radians(45.0f), static_cast<float>(details::kWindowWidth)
                                 / static_cast<float>(details::kWindowHeight),
        0.01f, 1000.0f);
    utility::UpdateUbo(ubo_, 64, 64, glm::value_ptr(proj_mat));
  }

 protected:
  void UpdateCameraVectors() {
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

  void MoveOnX() {
    position_ += direction_right_ * speed_ * gDeltaTime * move_vector_.x;
  }
  void MoveOnZ() {
    position_ += direction_front_ * speed_ * gDeltaTime * move_vector_.y;
  }

  glm::vec3 position_;
  glm::vec3 direction_front_;
  glm::vec3 direction_up_;
  glm::vec3 direction_right_;
  glm::vec3 direction_world_up_;

  float yaw_;
  float pitch_;
  //we don't need roll

  glm::vec2 move_vector_;
  GLuint ubo_{0};

  const float& speed_;
};

class GameCamera final : public ICamera {
 public:
  void ProcessMouseMovement(float xoffset, float yoffset) override {
    /* highlight the area under the cursor - done with some unknown magic */

    yaw_ += xoffset * sensitivity_;
    pitch_ += yoffset * sensitivity_;

    /// constrained pitch
    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    UpdateCameraVectors();
  }

 private:
  float rotation_speed_;
};

class EditorCamera final : public ICamera {
 public:
/*  EditorCamera(
      GLuint camera_ubo_id_,
      const float& player_speed)
      : CameraBase(camera_ubo_id_, player_speed) {}*/
  void ProcessMouseMovement(float xoffset, float yoffset) override {
    yaw_ += xoffset * sensitivity_;
    pitch_ += yoffset * sensitivity_;

    /// constrained pitch
    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    UpdateCameraVectors();
  }

 private:
  float sensitivity_;
};

class MenuCamera {};

class CameraHandler {
 public:
  // just create in-place
  void CameraHandler(); // idk, but we should create all cameras here

  //by pressing 'v' (like view...)
  void NextCamera() {
    if (++cur_id_ == gCamerasNum) {
      cur_id_ = 0;
    }
  }

  void ProcessMovement(float xoffset, float yoffset) {
    cameras_[cur_id_]->ProcessMovement(xoffset, yoffset);
  }
  void ProcessKeyInput() {
    cameras_[cur_id_]->ProcessKeyInput();
  }

  //TODO: need also to handle other camera effects... like rotation for GameCamera
  // maybe this..:
  ICamera* GetCurCamera() {
    return cameras_[cur_id_];
  }

  static constexpr int gCamerasNum{2};
  // ProcessCamera()
 // but how to switch needed camera: CameraHandler creates all needed cameras
 // and switch between them.

  //input: mouse

 private:
  int cur_id_{0};
  std::array<ICamera*, gCamerasNum> cameras_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_IO_CAMERAS_H_
