#include "RigidBody.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../../core/TileRenderer.h"
#include "../../io/Window.h"  // gDeltaTime

float GetYawFromQuat(const glm::quat& q) {
  return std::atan2(2.0f * (q.y * q.w + q.x * q.z),
                    1.0f - 2.0f * (q.y * q.y + q.x * q.x));
}

void RigidBody::UpdatePosition(UiSharedResources& ui_shared_resources,
                               glm::vec3 position_diff) {
  glm::vec2 pos_start(position_.x, position_.z);
  position_ += position_diff * speed_ * gDeltaTime;
  position_ = glm::clamp(position_, -32.0f, 32.0f);
  glm::vec2 pos_end(position_.x, position_.z);
  UpdatePositionY(ui_shared_resources);
  glm::vec2 move_dir = pos_end - pos_start;

  if (glm::length(move_dir) < 0.0001f) {
    return;  // no movement
  }

  float angle_start = GetYawFromQuat(rotation_);
  float angle_move = std::atan2(move_dir.x, move_dir.y);

  float delta = angle_move - angle_start;
  delta = std::remainder(delta, glm::two_pi<float>());
  //  delta = std::fmod(delta + glm::pi<float>(), glm::two_pi<float>()) -
  //  glm::pi<float>();

  float factor_lag = 10.0f * gDeltaTime;  // for lerp
  float new_yaw = angle_start + delta * factor_lag;

  rotation_ = glm::angleAxis(new_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 RigidBody::GenModelMat(UiSharedResources& ui_shared_resources,
                                 float scale) {
  auto map_scale =
      ui_shared_resources.gltf_context_.tile_renderer->cur_tile_.map_scale;
  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(object_model, position_);
  object_model *= glm::mat4_cast(rotation_);
  object_model = glm::scale(object_model, scale_ * scale);
  glm::mat4 map_model =
      glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));  // upscaled
  return map_model * object_model;
}

glm::mat4 RigidBody::GenModelMat(UiSharedResources& ui_shared_resources,
                                 float scale, GLuint position) {
  int fx = position >> 10;
  int fz = position & 1023;
  int x = fx / 16.0f - 32.0f;
  int z = fz / 16.0f - 32.0f;
  float ground_height = GetPositionY(ui_shared_resources, fx, fz);
  auto map_scale =
      ui_shared_resources.gltf_context_.tile_renderer->cur_tile_.map_scale;
  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(object_model, glm::vec3(x, ground_height, z));
  object_model = glm::scale(object_model, glm::vec3(scale));
  glm::mat4 map_model =
      glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));  // upscaled
  return map_model * object_model;
}

void RigidBody::UpdatePositionY(UiSharedResources& ui_shared_resources) {
  float fx = position_.x * 16.0f + 512.0f;
  float fy = position_.z * 16.0f + 512.0f;

  float ground_height = RigidBody::GetPositionY(ui_shared_resources, fx, fy);

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

float RigidBody::GetPositionY(UiSharedResources& ui_shared_resources, float fx,
                              float fy) {
  int x = static_cast<int>(floorf(fx));
  int y = static_cast<int>(floorf(fy));

  float tx = fx - x;  // 0..1
  float ty = fy - y;  // 0..1

  const auto& h = ui_shared_resources.gltf_context_.tile_renderer->cur_tile_
                      .terrain_heights_;
  int stride = 1024;

  // sample 4 neighbors
  float h00 = h[y * stride + x];
  float h10 = h[y * stride + x + 1];
  float h01 = h[(y + 1) * stride + x];
  float h11 = h[(y + 1) * stride + x + 1];

  // bilinear interpolation
  float hx0 = h00 + (h10 - h00) * tx;
  float hx1 = h01 + (h11 - h01) * tx;
  return hx0 + (hx1 - hx0) * ty;
}
void RigidBody::Jump(float strength) {
  if (is_on_ground_) {
    gravity_velocity_ = strength;
    is_on_ground_ = false;
  }
}
