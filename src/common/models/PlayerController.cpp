#include "PlayerController.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

PlayerController::PlayerController(
JPH::Ref<JPH::CharacterVirtual> jph_character,
CharacterSharedData* shared_data,
const Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::Skin* skin)
: character_(jph_character, shared_data, scene_node, model, skin) {
  character_.jph_character_->SetCharacterVsCharacterCollision(&shared_data->mCharacterVsCharacterCollision_);
  character_.jph_character_->SetListener(shared_data->contact_listener_);
}

void PlayerController::ProcessMovement(int key, int action) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_W) {
      SetMoveForward(true);
    } else if (key == GLFW_KEY_S) {
      SetMoveBackward(true);
    } else if (key == GLFW_KEY_A) {
      SetMoveLeft(true);
    } else if (key == GLFW_KEY_D) {
      SetMoveRight(true);
    } else if (key == GLFW_KEY_SPACE) {
      character_.TriggerJump();
    } else if (key == GLFW_KEY_EQUAL) {
      // Kick();
    } else if (key == GLFW_KEY_MINUS) {
      // Stunned();
    } else if (key == GLFW_KEY_T) {
      // Rest();
    } else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
      character_.Run();
    } else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
      character_.Crouch();
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W) {
      SetMoveForward(false);
    } else if (key == GLFW_KEY_S) {
      SetMoveBackward(false);
    } else if (key == GLFW_KEY_A) {
      SetMoveLeft(false);
    } else if (key == GLFW_KEY_D) {
      SetMoveRight(false);
    } else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
      character_.BackToWalk();
    } else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
      character_.BackToWalk();
    }
  }
  character_.movement_direction_ = GetMovementDirection();
}

JPH::Vec3 PlayerController::GetMovementDirection() {
  JPH::Vec3 movement_direction = JPH::Vec3::sZero();
  if (move_left_) movement_direction.SetZ(-1);
  if (move_right_) movement_direction.SetZ(1);
  if (move_forward_) movement_direction.SetX(1);
  if (move_backward_) movement_direction.SetX(-1);
  if (movement_direction != JPH::Vec3::sZero())
    movement_direction = movement_direction.Normalized();
  return movement_direction;
}
