#include "RigidBody.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../Details.h"
#include "ModelLoader.h"

float RigidBody::GetYawFromQuat(const glm::quat& q) {
  return std::atan2(2.0f * (q.y * q.w + q.x * q.z),
                    1.0f - 2.0f * (q.y * q.y + q.x * q.x));
}

RigidBody::RigidBody() { id_ = gEntityIdManager.Allocate(); }

glm::mat4 RigidBody::GenModelMat(float map_scale) {
  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(object_model, position_);
  object_model *= glm::mat4_cast(rotation_);
  object_model = glm::scale(object_model, scale_);
  glm::mat4 map_model =
      glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));  // upscaled
  return map_model * object_model;
}

Aabb3D RigidBody::GenWorldAabb() const noexcept {
  auto model_mat = glm::translate(glm::mat4(1.0f), GetPosition());
  model_mat = glm::scale(model_mat, GetScale());
  return Aabb3D::TransformAabb(model_data_->aabb, model_mat);
}

Obb3D RigidBody::GenWorldObb() const noexcept {
  auto model_mat = glm::translate(glm::mat4(1.0f), GetPosition());
  model_mat *= glm::mat4_cast(rotation_);
  model_mat = glm::scale(model_mat, GetScale());
  return AabbToObb(model_data_->aabb, model_mat);
}
