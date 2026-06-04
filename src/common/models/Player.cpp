#include "Player.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../../io/Window.h"
#include "Animator.h"

constexpr bool sPlayerCanPushOtherCharacters = true;
constexpr bool sOtherCharactersCanPushPlayer = true;

void Player::SetAnimator(Animator* animator) {
  animator_ = animator;
  animation_id_ = animator_->AddInstance();
}

void Player::Render() {
  // bool started_over = animator_->UpdateUbo(animation_id_, animation_time_);
  // if (started_over && !animation_looped_) {
  //   ResetState();
  // }


  // if reached time point OR animation's over (idle state)
  // if (!next_event_.done &&
      // (animation_time_ >= next_event_.time || state_ == State::kIdle)) {
    // next_event_.done = true;
    // FireEvent(next_event_.type);
  // }
  // model_data_->BindTextures();
  // auto model = GenModelMat(map_scale);
  // glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  // model_data_->RenderModelNodes();
}

void Player::Update(JPH::Vec3 camera_forward, JPH::Vec3 gravity) {
  // std::cout << camera_forward.GetX() << ' ' << camera_forward.GetY() << ' ' << camera_forward.GetZ() << std::endl;
  // Determine controller input
  JPH::Vec3 mControlInput = JPH::Vec3::sZero();
  if (move_left_) mControlInput.SetZ(-1);
  if (move_right_) mControlInput.SetZ(1);
  if (move_forward_) mControlInput.SetX(1);
  if (move_backward_) mControlInput.SetX(-1);
  if (mControlInput != JPH::Vec3::sZero())
    mControlInput = mControlInput.Normalized();

  // Rotate controls to align with the camera
  JPH::Vec3 cam_fwd = camera_forward;
  cam_fwd.SetY(0.0f);
  cam_fwd = cam_fwd.NormalizedOr(JPH::Vec3::sAxisX());
  JPH::Quat rotation = JPH::Quat::sFromTo(JPH::Vec3::sAxisX(), cam_fwd);
  mControlInput = rotation * mControlInput;

  JPH::Vec3 mDesiredVelocity = JPH::Vec3::sZero();

  if (mCharacter->IsSupported()) {
    mDesiredVelocity = mControlInput * speed_;
    mAllowSliding = !mControlInput.IsNearZero();
  } else {
    mAllowSliding = true;
  }
  JPH::Quat character_up_rotation = JPH::Quat::sEulerAngles(JPH::Vec3(0, 0, 0));
  mCharacter->SetUp(character_up_rotation.RotateAxisY());
  JPH::Quat target_rotation = JPH::Quat::sFromTo(JPH::Vec3::sAxisZ(), cam_fwd);
  mCharacter->SetRotation(target_rotation);

  mCharacter->UpdateGroundVelocity();
  JPH::Vec3 current_vertical_velocity = mCharacter->GetLinearVelocity().Dot(mCharacter->GetUp()) * mCharacter->GetUp();
  JPH::Vec3 ground_velocity = mCharacter->GetGroundVelocity();
  JPH::Vec3 new_velocity;
  bool moving_towards_ground = (current_vertical_velocity.GetY() - ground_velocity.GetY()) < 0.1f;
  if (mCharacter->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround &&
      (!mCharacter->IsSlopeTooSteep(mCharacter->GetGroundNormal()))) {
    new_velocity = ground_velocity;
    if (state_ != State::kIdle) {
      state_ = State::kIdle;
    }
    if (jump_triggered_ && moving_towards_ground) {
      new_velocity += 1 * jump_speed_  * mCharacter->GetUp();
      jump_triggered_ = false;
      state_ = State::kStunned;
    }
  } else
    new_velocity = current_vertical_velocity;

  // Gravity
  new_velocity += (character_up_rotation * gravity) * gDeltaTime;

  if (mCharacter->IsSupported()) {
    new_velocity += character_up_rotation * mDesiredVelocity;
  } else {
    JPH::Vec3 current_horizontal_velocity = mCharacter->GetLinearVelocity() - current_vertical_velocity;
    new_velocity += current_horizontal_velocity;
  }
  mCharacter->SetLinearVelocity(new_velocity);


  // --- animation walking
  if (state_ == State::kIdle) {
    const auto& animation_data = animator_->GetInstance(animation_id_);
    auto velocity_mag = new_velocity.Length();
    bool is_moving = velocity_mag > 0.01f;
    Animator::Type new_type;
    if (is_moving) {
      if (velocity_mag < 1.0f) {
        new_type = Animator::Type::kWalk;
      } else {
        new_type = Animator::Type::kRun;
      }
      // new_animationd_id = HumanAnimation::kCrouch;
    } else {
      new_type = Animator::Type::kIdle;
    }
    if (animation_data.type != new_type) {
      animator_->Start(animation_id_, new_type);
    }
  }
}

void Player::ProcessMovement(int key, int action) {
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
      jump_triggered_ = true;
      animator_->Start(animation_id_, Animator::Type::kJump);
    } else if (key == GLFW_KEY_EQUAL) {
      // Kick();
    } else if (key == GLFW_KEY_MINUS) {
      // Stunned();
    } else if (key == GLFW_KEY_T) {
      // Rest();
    } else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
      speed_ = default_speed_ * 2.0f;
    } else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
      switch_stance_triggered_ = true;
      speed_ = default_speed_ * 0.5f;
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
      speed_ = default_speed_;
    } else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
      speed_ = default_speed_;
      switch_stance_triggered_ = true;
    }
  }
}
