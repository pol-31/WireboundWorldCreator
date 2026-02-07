#include "Fpv.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../../io/Window.h"
#include "../Details.h"
#include "ModelLoader.h"

void Fpv::Render(float map_scale) {
  model_data_->BindTextures();
  auto model = GenModelMat(map_scale);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
}

void Fpv::Update(UiRenderData& render_data) {
  auto position_diff = glm::vec3{0.0f};
  glm::vec2 pos_start(position_.x, position_.z);
  position_ += position_diff * speed_ * gDeltaTime;
  position_ = glm::clamp(position_, -32.0f, 32.0f);
  glm::vec2 pos_end(position_.x, position_.z);
  float ground_height =
      render_data.glfw_context_.tile_renderer
      ->cur_tile_.GetPositionY32(position_.x, position_.z);
  position_.y = ground_height;

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
