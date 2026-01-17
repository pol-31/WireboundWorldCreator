#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_RIGIDBODY_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_RIGIDBODY_H_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../modes/UiSharedResources.h"
#include "../EntityIdManager.h"
#include "Aabb3D.h"
#include "Animation.h"
#include "AttackEvent.h"
#include "ModelLoader.h"

class RigidBody {
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

  RigidBody() = default;

  void UpdatePosition(UiSharedResources& ui_shared_resources,
                      glm::vec3 position_diff);

  void Jump(float strength);

  void Kick();

  void Stunned();

  void Fall();  // internally called by ApplyGravity()

  void ApplyGravity(UiSharedResources& ui_shared_resources);

  void ResetState();

  [[nodiscard]] const glm::vec3& GetPosition() const noexcept {
    return position_;
  }
  void SetPosition(const glm::vec3& position) { position_ = position; }
  [[nodiscard]] const glm::quat& GetRotation() const noexcept {
    return rotation_;
  }
  void SetRotation(const glm::quat& rotation) { rotation_ = rotation; }
  [[nodiscard]] const glm::vec3& GetScale() const noexcept { return scale_; }
  void SetScale(const glm::vec3& scale) { scale_ = scale; }

  void UpdatePositionY(UiSharedResources& ui_shared_resources);

  glm::mat4 GenModelMat(UiSharedResources& ui_shared_resources, float scale);

  [[nodiscard]] const ModelData* GetModelData() const noexcept {
    return model_data_;
  }

  void SetModelData(ModelData* model, HumanAnimator* animator,
                    std::vector<AttackEvent>* attack_queue) {
    model_data_ = model;
    animator_ = animator;
    attack_queue_ = attack_queue;
  }

  [[nodiscard]] uint32_t GetId() const noexcept { return id_; }

  Aabb3D GenWorldAabb() const noexcept;

  void RenderPicking(UiSharedResources& ui_shared_resources);

 protected:
  void UpdateAnimation();

  uint32_t id_ = EntityIdManager::InvalidEntityId;

  State state_ = State::kIdle;

  glm::vec3 position_ = glm::vec3(0.0f);
  glm::quat rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 scale_ = glm::vec3(1.0f);

  float gravity_velocity_ = 0.0f;
  float speed_ = 10.0f;

  HumanAnimator::Type animation_id_ =
      HumanAnimator::Type::kIdle;  // TODO: merge with state_
  float animation_time_ = 0.0f;
  bool animation_looped_ = true;

  ModelData* model_data_ = nullptr;
  HumanAnimator* animator_ = nullptr;
  std::vector<AttackEvent>* attack_queue_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_RIGIDBODY_H_
