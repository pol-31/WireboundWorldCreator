#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_HUMAN_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_HUMAN_H_

#include <glm/glm.hpp>

#include "../../ui/UiRenderData.h"
#include "Animator.h"
#include "RigidBody.h"

struct ModelData;

class Human : public RigidBody {
 public:
  enum class State {
    kIdle,
    kWalking,
    kRunning,
    kCrouching,
    kJumping,
    kFalling,
    kAttacking,
    kStunned
  };

  [[nodiscard]] bool IsOnGround() const noexcept {
    return state_ != State::kJumping && state_ != State::kFalling;
  }

  [[nodiscard]] bool IsRelaxed() const noexcept {
    return state_ != State::kJumping && state_ != State::kFalling &&
           state_ != State::kAttacking && state_ != State::kStunned;
  }

  Human() = default;

  void UpdatePosition(UiRenderData& render_data, glm::vec3 position_diff);

  void Jump(float strength);

  void Kick();

  void Stunned() override;

  void Fall();  // internally called by ApplyGravity()

  void ApplyGravity(UiRenderData& render_data);

  void ResetState();

  void UpdatePositionY(UiRenderData& render_data);

  // should be called in Render(), it updates skin ubo
  void UpdateAnimation();

  void Render(float map_scale);

  void Update(UiRenderData& render_data);

  void Select() { selected_ = true; }

  void DeSelect() { selected_ = false; }

  [[nodiscard]] bool IsSelected() const noexcept { return selected_; }

  void SetAnimator(Animator* animator) { animator_ = animator; }

 private:
  State state_ = State::kIdle;
  Animation animation_id_ = Animation::kIdle;  // TODO: merge with state_
  bool selected_ = false;

  float animation_time_ = 0.0f;
  bool animation_looped_ = true;
  Animator* animator_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_HUMAN_H_
