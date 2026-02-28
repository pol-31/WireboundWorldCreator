#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_PLAYERHUMAN_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_PLAYERHUMAN_H_

#include "../../ui/UiRenderData.h"
#include "Animator.h"
#include "PlayerFpv.h"
#include "RigidBody.h"

class PlayerHuman : public RigidBody {
 public:
  enum class State { kIdle, kJumping, kFalling, kAttacking, kStunned, kFpv };

  enum class PlayerEventType {
    None,
    DealDamage,
    OpenUI,
    EnterIdle,
    EnterIdleSitting,
  };

  void FireEvent(PlayerEventType e);

  void DealDamage();

  void EnterIdle();

  void EnterIdleSitting();

  struct Event {
    float time = 0.f;
    PlayerEventType type = PlayerEventType::None;
    bool done = true;
  };

  Event next_event_;

  [[nodiscard]] bool IsOnGround() const noexcept {
    return state_ != State::kJumping && state_ != State::kFalling;
  }

  [[nodiscard]] bool IsIdle() const noexcept { return state_ == State::kIdle; }

  [[nodiscard]] bool IsFpv() const noexcept { return state_ == State::kFpv; }

  PlayerHuman(UiRenderData& render_data);

  void Jump(float strength);

  void Kick();

  void Rest();

  void Stunned() override;

  void Fall();  // internally called by ApplyGravity()

  void ApplyGravity(UiRenderData& render_data);

  void ResetState();

  // should be called in Render(), it updates skin ubo
  void UpdateAnimation();

  void RenderHuman(float map_scale);

  void RenderFpv(float map_scale);

  void Update(UiRenderData& render_data);

  void ProcessMovement(int key, int action);

  void SwitchToHuman();

  void SwitchToFpv();

  PlayerFpv& GetFpv() { return fpv_; }

  void SetAnimator(Animator* animator) { animator_ = animator; }

  void SetMoveForward(bool pressed) { move_forward_ = pressed; }
  void SetMoveBackward(bool pressed) { move_backward_ = pressed; }
  void SetMoveLeft(bool pressed) { move_left_ = pressed; }
  void SetMoveRight(bool pressed) { move_right_ = pressed; }

 private:
  State state_ = State::kIdle;

  bool move_forward_ = false;
  bool move_backward_ = false;
  bool move_left_ = false;
  bool move_right_ = false;

  glm::vec3 velocity_ = glm::vec3(0.0f);
  glm::vec3 desired_direction_ = glm::vec3(0.0f);
  float desired_magnitude_ = 0.0f;
  float acceleration_ = 5.0f;

  float animation_time_ = 0.0f;
  bool animation_looped_ = true;
  Animator* animator_ = nullptr;
  Animation animation_id_ = Animation::kIdle;  // todo; merge with state_?

  PlayerFpv fpv_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_PLAYERHUMAN_H_
