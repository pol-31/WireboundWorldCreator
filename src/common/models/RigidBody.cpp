#include "RigidBody.h"

#include "../../core/TileRenderer.h"
#include "../../io/Window.h" // gDeltaTime
#include <glm/gtc/matrix_transform.hpp>

float GetYawFromQuat(const glm::quat& q) {
  return std::atan2(2.0f * (q.y * q.w + q.x * q.z),
                    1.0f - 2.0f * (q.y * q.y + q.x * q.x));
}

void RigidBody::UpdatePosition(
    UiSharedResources& ui_shared_resources,
    glm::vec3 position_diff) {
  glm::vec2 pos_start(position_.x, position_.z);
  position_ += position_diff * speed_ * gDeltaTime;
  position_ = glm::clamp(position_, -32.0f, 32.0f);
  glm::vec2 pos_end(position_.x, position_.z);
  UpdatePositionY(ui_shared_resources);
  glm::vec2 move_dir = pos_end - pos_start;

  if (glm::length(move_dir) < 0.0001f) {
    return; // no movement
  }

  float angle_start = GetYawFromQuat(rotation_);
  float angle_move = std::atan2(move_dir.x, move_dir.y);

  float delta = angle_move - angle_start;
  delta = std::remainder(delta, glm::two_pi<float>());
  //  delta = std::fmod(delta + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();

  float factor_lag = 10.0f * gDeltaTime; // for lerp
  float new_yaw = angle_start + delta * factor_lag;

  rotation_ = glm::angleAxis(new_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 RigidBody::GenModelMat(
    UiSharedResources& ui_shared_resources, float scale) {
  auto map_scale = ui_shared_resources.global_glfw_callback_data_
                       .tile_renderer->cur_tile_.map_scale;
  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(object_model, position_);
  object_model *= glm::mat4_cast(rotation_);
  object_model = glm::rotate(object_model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  object_model = glm::rotate(object_model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  object_model = glm::scale(object_model, glm::vec3(scale));
  glm::mat4 map_model = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale)); //upscaled
  return map_model * object_model;
}

void RigidBody::UpdatePositionY(UiSharedResources& ui_shared_resources) {
  int x = static_cast<int>(position_.x * 16.0f + 512.0f);
  int y = static_cast<int>(position_.z * 16.0f + 512.0f);
  float ground_height =
      ui_shared_resources.global_glfw_callback_data_
          .tile_renderer->cur_tile_.terrain_heights_[y * 1024 + x] + 1.0f;
  if (!is_on_ground_) {
    const float gravity = -9.81f;
    const float mass = 4.0f;
    gravity_velocity_ += gDeltaTime * gravity;
    position_.y += gravity_velocity_ * gDeltaTime * mass;
    if (position_.y <= ground_height) {
      position_.y = ground_height;
      gravity_velocity_ = 0.0f;
      is_on_ground_ = true;
    }
  } else {
    position_.y = ground_height;
  }
}

void RigidBody::Jump(float strength) {
  if (is_on_ground_) {
    gravity_velocity_ = strength;
    is_on_ground_ = false;
  }
}
