#ifndef WIREBOUNDWORLDCREATOR_PLAYERFPV_H
#define WIREBOUNDWORLDCREATOR_PLAYERFPV_H

#include "../../modes/UiSharedResources.h"
#include "PlayerBase.h"

class PlayerFpv : public PlayerBase {
 public:
  enum class State {
    kIdle,
    kWalking,
    kRunning,
    kCrouching,
    kJumping,
    kFalling,
    kAttacking,
    kStunned,
    kOnPlayer
  };

  [[nodiscard]] bool IsOnGround() const noexcept {
    return state_ != State::kJumping && state_ != State::kFalling;
  }

  [[nodiscard]] bool IsRelaxed() const noexcept {
    return state_ != State::kJumping && state_ != State::kFalling &&
           state_ != State::kAttacking && state_ != State::kStunned;
  }

  [[nodiscard]] bool IsReadyToSwitch() const noexcept {
    return IsRelaxed() || state_ == State::kOnPlayer;
  }

  PlayerFpv(UiSharedResources& ui_shared_resources);

  void SetRotateCcw(bool pressed) { rotate_ccw_ = pressed; }
  void SetRotateCw(bool pressed) { rotate_cw_ = pressed; }
  void SetThrustUp(bool pressed) { thrust_up_ = pressed; }
  void SetThrustDown(bool pressed) { thrust_down_ = pressed; }

  void Stunned() override;

  void ApplyGravity(UiSharedResources& ui_shared_resources);

  void ResetState();

  void UpdatePositionY(UiSharedResources& ui_shared_resources);

  void RenderPicking(UiSharedResources& ui_shared_resources);

  // should be called in Render(), it updates skin ubo
  void UpdateAnimation();

  void Render(UiSharedResources& ui_shared_resources);

  void Update(UiSharedResources& ui_shared_resources);

  void ProcessMovement(int key, int action) override;

  void SetAnimation(FpvAnimation type, float time, bool looped);

  void SwitchToHuman();

  void SwitchToFpv();

  void ResetRotation();

 private:
  State state_ = State::kIdle;
  FpvAnimation animation_id_ = FpvAnimation::kIdle;  // TODO: merge with state_

  glm::vec3 angular_velocity = glm::vec3(0.0f);  // radians/sec (local)
  glm::vec3 euler_angles = glm::vec3(0.0f);      // yaw, pitch, roll

  float throttle_ = 0.0f;
  float yaw_rate_ = 0.0f;
  float pitch_rate_ = 0.0f;
  float roll_rate_ = 0.0f;

  bool rotate_ccw_ = false;
  bool rotate_cw_ = false;
  bool thrust_up_ = false;
  bool thrust_down_ = false;
};

#endif  // WIREBOUNDWORLDCREATOR_PLAYERFPV_H
