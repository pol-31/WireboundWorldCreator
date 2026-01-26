#ifndef WIREBOUNDWORLDCREATOR_FPV_H
#define WIREBOUNDWORLDCREATOR_FPV_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../modes/UiSharedResources.h"
#include "Animator.h"
#include "RigidBody.h"

class Fpv : public RigidBody {
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

  Fpv() = default;

  void UpdatePosition(UiSharedResources& ui_shared_resources,
                      glm::vec3 position_diff);

  void Jump(float strength);

  void Kick();

  void Stunned() override;

  void Fall();  // internally called by ApplyGravity()

  void ApplyGravity(UiSharedResources& ui_shared_resources);

  void ResetState();

  void UpdatePositionY(UiSharedResources& ui_shared_resources);

  void RenderPicking(UiSharedResources& ui_shared_resources);

  // should be called in Render(), it updates skin ubo
  void UpdateAnimation();

  void Render(UiSharedResources& ui_shared_resources);

  void Update(UiSharedResources& ui_shared_resources);

  void Select() { selected_ = true; }

  void DeSelect() { selected_ = false; }

  [[nodiscard]] bool IsSelected() const noexcept { return selected_; }

 protected:
  State state_ = State::kIdle;
  FpvAnimation animation_id_ = FpvAnimation::kIdle;  // TODO: merge with state_
  bool selected_ = false;
};

#endif  // WIREBOUNDWORLDCREATOR_FPV_H
