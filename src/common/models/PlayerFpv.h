#ifndef WIREBOUNDWORLDCREATOR_PLAYERFPV_H
#define WIREBOUNDWORLDCREATOR_PLAYERFPV_H

#include "../../modes/UiSharedResources.h"
#include "RigidBody.h"

class PlayerFpv : public RigidBody {
 public:
  PlayerFpv(UiSharedResources& ui_shared_resources);

  void SetMoveForward(bool pressed) { move_forward_ = pressed; }
  void SetMoveBackward(bool pressed) { move_backward_ = pressed; }
  void SetMoveLeft(bool pressed) { move_left_ = pressed; }
  void SetMoveRight(bool pressed) { move_right_ = pressed; }
  void SetRotateCcw(bool pressed) { rotate_ccw_ = pressed; }
  void SetRotateCw(bool pressed) { rotate_cw_ = pressed; }
  void SetThrustUp(bool pressed) { thrust_up_ = pressed; }
  void SetThrustDown(bool pressed) { thrust_down_ = pressed; }

  [[nodiscard]] const glm::vec3& GetVelocity() const noexcept {
    return velocity_;
  }

  void Render(UiSharedResources& ui_shared_resources);

  void RenderRigged(UiSharedResources& ui_shared_resources);

  void Update(UiSharedResources& ui_shared_resources);

  void ProcessMovement(int key, int action);

  void ResetRotation();

 private:
  bool move_forward_ = false;
  bool move_backward_ = false;
  bool move_left_ = false;
  bool move_right_ = false;
  bool rotate_ccw_ = false;
  bool rotate_cw_ = false;
  bool thrust_up_ = false;
  bool thrust_down_ = false;

  glm::vec3 velocity_ = glm::vec3(0.0f);
  glm::vec3 desired_direction_ = glm::vec3(0.0f);
  float desired_magnitude_ = 0.0f;
  float acceleration_ = 5.0f;

  glm::vec3 angular_velocity = glm::vec3(0.0f);  // radians/sec (local)
  glm::vec3 euler_angles = glm::vec3(0.0f);      // yaw, pitch, roll

  float throttle_ = 0.0f;
  float yaw_rate_ = 0.0f;
  float pitch_rate_ = 0.0f;
  float roll_rate_ = 0.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_PLAYERFPV_H
