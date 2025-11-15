#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_RIGIDBODY_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_RIGIDBODY_H_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../modes/UiSharedResources.h"

class RigidBody {
 public:
  RigidBody() = default;

  void UpdatePosition(
      UiSharedResources& ui_shared_resources, glm::vec3 position_diff);

  void Jump(float strength);

  void SetPosition(glm::vec3 position) {
    position_ = position;
  }

  [[nodiscard]] glm::vec3 GetPosition() const noexcept {
    return position_;
  }

 protected:
  glm::mat4 GenModelMat(UiSharedResources& ui_shared_resources, float scale);

  void UpdatePositionY(UiSharedResources& ui_shared_resources);

  glm::vec3 position_ = glm::vec3(0.0f);
  glm::quat rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 scale_ = glm::vec3(1.0f);

  bool is_on_ground_ = true;
  float gravity_velocity_ = 0.0f;
  float speed_ = 10.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_RIGIDBODY_H_
