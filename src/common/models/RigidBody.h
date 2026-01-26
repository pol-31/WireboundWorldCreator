#ifndef WIREBOUNDWORLDCREATOR_RIGIDBODY_H
#define WIREBOUNDWORLDCREATOR_RIGIDBODY_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../modes/UiSharedResources.h"
#include "../EntityIdManager.h"
#include "Animator.h"
#include "AttackEvent.h"

class ModelData;

class RigidBody {
 public:
  RigidBody();

  virtual ~RigidBody() = default;

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

  glm::mat4 GenModelMat(UiSharedResources& ui_shared_resources, float scale);

  [[nodiscard]] const ModelData* GetModelData() const noexcept {
    return model_data_;
  }

  void SetModelData(ModelData* model, Animator* animator,
                    std::vector<AttackEvent>* attack_queue) {
    model_data_ = model;
    animator_ = animator;
    attack_queue_ = attack_queue;
  }

  [[nodiscard]] uint32_t GetId() const noexcept { return id_; }

  Aabb3D GenWorldAabb() const noexcept;

  Obb3D GenWorldObb() const noexcept;

  virtual void Stunned() {}  // TODO: = 0

 protected:
  static float GetYawFromQuat(const glm::quat& q);

  uint32_t id_ = EntityIdManager::InvalidEntityId;

  glm::vec3 position_ = glm::vec3(0.0f);
  glm::quat rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 scale_ = glm::vec3(1.0f);

  float gravity_velocity_ = 0.0f;
  float speed_ = 10.0f;

  float animation_time_ = 0.0f;
  bool animation_looped_ = true;

  ModelData* model_data_ = nullptr;
  Animator* animator_ = nullptr;
  std::vector<AttackEvent>* attack_queue_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_RIGIDBODY_H
