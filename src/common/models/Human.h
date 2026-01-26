#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_HUMAN_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_HUMAN_H_

#include <glm/glm.hpp>

#include "../../modes/UiSharedResources.h"
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

 private:
  State state_ = State::kIdle;
  HumanAnimation animation_id_ =
      HumanAnimation::kIdle;  // TODO: merge with state_
  bool selected_ = false;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_HUMAN_H_
