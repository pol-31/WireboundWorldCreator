#include "Player.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../io/Camera.h"
#include "../../core/TileRenderer.h"
#include "ModelLoader.h"

const float Player::gSpeedSlow = 5.0f;
const float Player::gSpeedModerate = 10.0f;
const float Player::gSpeedFast = 20.0f;

Player::Player(UiSharedResources& ui_shared_resources)
    : camera_(ui_shared_resources.global_glfw_callback_data_.camera) {
  speed_ = gSpeedModerate;
}

void Player::Render(UiSharedResources& ui_shared_resources) {
  ui_shared_resources.shader_model_.Bind();
  model_data_->BindTextures();
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  glBindVertexArray(model_data_->vao);
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

void Player::Update(UiSharedResources& ui_shared_resources) {
  auto pos_diff = front_move_to_ + side_move_to_;
  if (front_move_to_ != glm::vec3(0.0f) &&
      side_move_to_ != glm::vec3(0.0f)) {
    pos_diff /= 2.0f; // otherwise double speed
  }
  UpdatePosition(ui_shared_resources, pos_diff);
  UpdateCamera(ui_shared_resources);
}

void Player::UpdateCamera(UiSharedResources& ui_shared_resources) {
  auto map_scale = ui_shared_resources.global_glfw_callback_data_
                       .tile_renderer->cur_tile_.map_scale;
  camera_->SetOrigin(GetPosition() * map_scale);
  camera_->MoveRotateViewOriginDist(0.0f); // update camera vectors
  camera_->Update(1.0f);
}

void Player::SetMoveForward(float magnitude) {
  move_forward_ = true;
  glm::vec3 world_front = camera_->GetDirectionWorldFront();
  front_move_to_ = -magnitude * world_front;
}

void Player::SetMoveBackward(float magnitude) {
  move_forward_ = false;
  glm::vec3 world_front = camera_->GetDirectionWorldFront();
  front_move_to_ = magnitude * world_front;
}

void Player::SetMoveLeft(float magnitude) {
  move_left_ = true;
  glm::vec3 world_right = camera_->GetDirectionRight();
  side_move_to_ = -magnitude * world_right;
}

void Player::SetMoveRight(float magnitude) {
  move_left_ = false;
  glm::vec3 world_right = camera_->GetDirectionRight();
  side_move_to_ = magnitude * world_right;
}

void Player::ResetMoveForward() {
  if (move_forward_) {
    front_move_to_ = glm::vec3(0.0f);
  }
}

void Player::ResetMoveBackward() {
  if (!move_forward_) {
    front_move_to_ = glm::vec3(0.0f);
  }
}

void Player::ResetMoveLeft() {
  if (move_left_) {
    side_move_to_ = glm::vec3(0.0f);
  }
}

void Player::ResetMoveRight() {
  if (!move_left_) {
    side_move_to_ = glm::vec3(0.0f);
  }
}

void Player::UpdateMods(bool mod_ctrl, bool mod_shift) {
  if (mod_ctrl) {
    speed_ = gSpeedSlow;
  } else if (mod_shift) {
    speed_ = gSpeedFast;
  } else {
    speed_ = gSpeedModerate;
  }
}
