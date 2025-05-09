#ifndef WIREBOUNDWORLDCREATOR_SRC_IO_CAMERAS_H_
#define WIREBOUNDWORLDCREATOR_SRC_IO_CAMERAS_H_

#include <array>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Cursor.h"

/// camera move:
/// left x-
/// right x+
/// forward y+
/// back y-

class ICamera {
 public:
  /*player/map speed*/
  ICamera(GLuint ubo);

  //position
  void SetMoveForward(float magnitude);
  void SetMoveBackward(float magnitude);
  void SetMoveLeft(float magnitude);
  void SetMoveRight(float magnitude);

  //direction TODO: time-based?
  void SetRotateOnYaw(float angle, float delay);

/*  void Update(float deltaTime) {
    // Compute the movement force
    glm::vec3 displacement = target_position_ - position_;

    // Apply damping (spring-like effect)
    velocity_ += displacement * damping_factor_ * deltaTime;
    velocity_ *= 1.0f - (damping_factor_ * deltaTime);  // Damping

    // Integrate velocity to update position
    position_ += velocity_ * deltaTime;
  }*/

  void SetRotateOnPitch(float angle, float delay);
  //TODO; but... is it bind to player character... I mean camera always
  // should lag behind the player and then recover after player stop...
  // Need to think more about it....

  //combinated
  void SmoothRotation(glm::vec3 target_pos, float yaw, float pitch,
                      float seconds);

  // We can implement them, but it doesn't mean we have to use it:
  // mouse key      | never
  // key            | + sometimes
  // scroll         | + sometimes
  // mouse movement | +

  /// camera interaction (not as glfw callbacks, because camera is only
  /// a part of the user interaction)
  virtual void ProcessMouseMovement(float xoffset, float yoffset) = 0;
  virtual void ProcessMouseKey(int button, int action, int mods) = 0;
  virtual void ProcessMouseScroll(float yoffset) = 0;
  virtual void ProcessKey(int key, int scancode, int action, int mods) = 0;


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

  void UpdateFrame();

  void UpdateConfig();

  void UpdateViewMatrix() const;

  //TODO: should be called from WindowSizeCallback
  void UpdateProjectionMatrix() const;

  void UpdateCameraVectors();

 protected:
  void MoveOnX();
  void MoveOnZ();

  glm::vec3 position_;
  glm::vec3 direction_front_;
  glm::vec3 direction_up_;
  glm::vec3 direction_right_;
  glm::vec3 direction_world_up_;

  float yaw_;
  float pitch_;
  //we don't need roll

  glm::vec2 move_vector_;

  float speed_;
  GLuint ubo_{0};
//  const float& speed_;
};

class InspectCamera final : public ICamera {
 public:
  using ICamera::ICamera;
  void ProcessMouseMovement(float xoffset, float yoffset) override;

  void ProcessMouseKey(int button, int action, int mods) override {}
  void ProcessMouseScroll(float yoffset) override {}
  void ProcessKey(int key, int scancode, int action, int mods) override {}

 private:
  float sensitivity_{0.2f};
};

class GameCamera final : public ICamera {
 public:
  using ICamera::ICamera;
  void ProcessMouseMovement(float xoffset, float yoffset) override;

  void ProcessMouseKey(int button, int action, int mods) override {}
  void ProcessMouseScroll(float yoffset) override {}
  void ProcessKey(int key, int scancode, int action, int mods) override {}

 private:
  float rotation_speed_;
  float sensitivity_{0.2f};
};

class MapCamera final : public ICamera {
 public:
  MapCamera(GLuint ubo);
  //TODO: instead of mode "tile" we have a map (press 'm')
  void ProcessMouseMovement(float xoffset, float yoffset) override;

  void ProcessMouseKey(int button, int action, int mods) override {}
  void ProcessMouseScroll(float yoffset) override {}
  void ProcessKey(int key, int scancode, int action, int mods) override {}
};

//TODO: add all other ICameraBase interface methods
// is it aggregate?
class CameraHandler {
 public:
  // just create in-place
  CameraHandler();

  ~CameraHandler();

  /// in case of window resize
  void UpdateProjectionMatrices();

  void ToggleCursor();

  void Update();

  void ProcessMouseMovement(float xoffset, float yoffset);

  void ProcessMouseKey(int button, int action, int mods);

  void ProcessMouseScroll(float yoffset);

  void ProcessKey(int key, int scancode, int action, int mods);

  //TODO: need also to handle other camera effects... like rotation for GameCamera
  // maybe this..:
  ICamera* GetCurCamera();

  void SetInspectCamera();

  void SetGameCamera();

  void SetMapCamera();

  // ICamera wrap

  void SetMoveForward(float magnitude = 1.0f) {
    cameras_[cur_id_]->SetMoveForward(magnitude);
  }

  void SetMoveBackward(float magnitude = 1.0f) {
    cameras_[cur_id_]->SetMoveBackward(magnitude);
  }

  void SetMoveLeft(float magnitude = 1.0f) {
    cameras_[cur_id_]->SetMoveLeft(magnitude);
  }

  void SetMoveRight(float magnitude = 1.0f) {
    cameras_[cur_id_]->SetMoveRight(magnitude);
  }

  void SetRotateOnYaw(float angle, float delay) {
    cameras_[cur_id_]->SetRotateOnYaw(angle, delay);
  }

  void SetRotateOnPitch(float angle, float delay) {
    cameras_[cur_id_]->SetRotateOnPitch(angle, delay);
  }

  void SmoothRotation(glm::vec3 target_pos, float yaw, float pitch,
                      float seconds) {
    cameras_[cur_id_]->SmoothRotation(target_pos, yaw, pitch, seconds);
  }

  void SetYaw(float yaw) {
    cameras_[cur_id_]->SetYaw(yaw);
  }

  void SetPitch(float pitch) {
    cameras_[cur_id_]->SetPitch(pitch);
  }

  void SetPosition(glm::vec3 position) {
    cameras_[cur_id_]->SetPosition(position);
  }

  static constexpr int gCamerasNum{3};

 private:
  void InitUbo();

  void DeInitUbo();

  void SetCamera(int id);

  int cur_id_{0};
  GLuint ubo_{0};

  std::array<std::unique_ptr<ICamera>, gCamerasNum> cameras_;
  Cursor cursor_;

  bool do_show_cursor_{true};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_IO_CAMERAS_H_
