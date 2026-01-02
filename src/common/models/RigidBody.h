#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_RIGIDBODY_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_RIGIDBODY_H_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../modes/UiSharedResources.h"

class RigidBody {
 public:
  RigidBody() = default;

  void UpdatePosition(UiSharedResources& ui_shared_resources,
                      glm::vec3 position_diff);

  void Jump(float strength);

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

  static float GetPositionY(UiSharedResources& ui_shared_resources, float fx,
                            float fy);

  glm::mat4 GenModelMat(UiSharedResources& ui_shared_resources, float scale);

  glm::mat4 GenModelMat(UiSharedResources& ui_shared_resources, float scale,
                        GLuint position);

 protected:
  glm::vec3 position_ = glm::vec3(0.0f);
  glm::quat rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  glm::vec3 scale_ = glm::vec3(1.0f);

  bool is_on_ground_ = true;
  float gravity_velocity_ = 0.0f;
  float speed_ = 10.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_RIGIDBODY_H_
