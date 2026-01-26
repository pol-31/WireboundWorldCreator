#include "Fpv.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../../io/Window.h"
#include "../Details.h"
#include "ModelLoader.h"

void Fpv::Render(UiSharedResources& ui_shared_resources) {
  UpdateAnimation();
  ui_shared_resources.shader_animated_mdl_.Bind();
  model_data_->BindTextures();
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  if (selected_) {
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
  }
  model_data_->RenderModelNodes();
  if (selected_) {
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    ui_shared_resources.shader_animated_mdl_selected_.Bind();
    model = GenModelMat(ui_shared_resources, 0.11f);
    glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
    model_data_->RenderModelNodes();
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
  }
  glBindVertexArray(0);
}

void Fpv::Update(UiSharedResources& ui_shared_resources) {
  UpdatePosition(ui_shared_resources, glm::vec3{0.0f});
}

void Fpv::RenderPicking(UiSharedResources& ui_shared_resources) {
  // TODO: render capsule model instead
  UpdateAnimation();
  ui_shared_resources.shader_animated_mdl_picking_.Bind();
  glUniform1ui(1, static_cast<uint32_t>(details::kIdOffsetObjects + id_));
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

// should be called in Render(), it updates skin ubo
void Fpv::UpdateAnimation() {
  bool started_over = animator_->UpdateUbo(animation_id_, animation_time_);
  if (started_over && !animation_looped_) {
    ResetState();
  }
}

void Fpv::UpdatePosition(UiSharedResources& ui_shared_resources,
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

void Fpv::UpdatePositionY(UiSharedResources& ui_shared_resources) {
  float ground_height =
      ui_shared_resources.glfw_context_.tile_renderer->cur_tile_.GetPositionY(
          position_.x * 16.0f + 512.0f, position_.z * 16.0f + 512.0f);
  if (!IsOnGround()) {
    const float gravity = -9.81f;
    const float mass = 4.0f;
    gravity_velocity_ += gDeltaTime * gravity;
    position_.y += gravity_velocity_ * gDeltaTime * mass;
    if (position_.y <= ground_height) {
      position_.y = ground_height;
      gravity_velocity_ = 0.0f;
      ResetState();
    }
  } else {
    position_.y = ground_height;
  }
}

void Fpv::Jump(float strength) {
  if (IsRelaxed()) {
    gravity_velocity_ = strength;
    state_ = State::kJumping;
    animation_time_ = 0.0f;
    animation_id_ = FpvAnimation::kJump;
    animation_looped_ = false;
  }
}

void Fpv::Kick() {
  if (IsRelaxed()) {
    state_ = State::kAttacking;
    animation_time_ = 0.0f;
    animation_id_ = FpvAnimation::kKick;
    animation_looped_ = false;
    AttackEvent attack;
    attack.attacker = id_;
    attack.damage = 1.0f;
    attack.hitbox = GenWorldAabb();
    attack_queue_->push_back(attack);
  }
}

void Fpv::Stunned() {
  if (IsRelaxed()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = FpvAnimation::kStunned;
    animation_looped_ = false;
  }
}

void Fpv::Fall() {
  if (state_ != State::kFalling) {
    state_ = State::kFalling;
    animation_time_ = 0.0f;
    animation_id_ = FpvAnimation::kFall;
    animation_looped_ = true;
  }
}

void Fpv::ApplyGravity(UiSharedResources& ui_shared_resources) {
  float ground_height =
      ui_shared_resources.glfw_context_.tile_renderer->cur_tile_.GetPositionY(
          position_.x * 16.0f + 512.0f, position_.z * 16.0f + 512.0f);
  if (!IsOnGround()) {
    const float gravity = -9.81f;
    const float mass = 1.0f;
    gravity_velocity_ += gDeltaTime * gravity;
    position_.y += gravity_velocity_ * gDeltaTime * mass;
    if (position_.y <= ground_height) {
      position_.y = ground_height;
      gravity_velocity_ = 0.0f;
      ResetState();
    } else {
      if (gravity_velocity_ <= 0.0f) {
        Fall();
      }
    }
  } else {
    position_.y = ground_height;
  }
}

void Fpv::ResetState() {
  state_ = State::kIdle;
  animation_id_ = FpvAnimation::kIdle;
  animation_time_ = 0.0f;
  animation_looped_ = true;
}
