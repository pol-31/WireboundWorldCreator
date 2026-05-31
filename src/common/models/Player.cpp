#include "Player.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../../io/Window.h"

constexpr bool sPlayerCanPushOtherCharacters = true;
constexpr bool sOtherCharactersCanPushPlayer = true;

Player::Player() {
  glCreateBuffers(1, &ubo_);
  glNamedBufferStorage(ubo_, sizeof(glm::vec3), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glBindBufferBase(GL_UNIFORM_BUFFER, 4, ubo_);
}

Player::~Player() {
  //
}

void Player::UpdateUbo() {
  // glNamedBufferSubData(ubo_, 0, sizeof(glm::vec3),
                       // glm::value_ptr(GetPosition()));
}

void Player::Render(float map_scale) {
  UpdateAnimation();
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

  // ---

  JPH::Vec3 mDesiredVelocity = JPH::Vec3::sZero();

  if (mCharacter->IsSupported()) {
    mDesiredVelocity = mControlInput * speed_;
    mAllowSliding = !mControlInput.IsNearZero();
  } else {
    mAllowSliding = true;
  }

  JPH::Quat character_up_rotation = JPH::Quat::sEulerAngles(JPH::Vec3(0, 0, 0));
  mCharacter->SetUp(character_up_rotation.RotateAxisY());
  mCharacter->SetRotation(character_up_rotation);

  mCharacter->UpdateGroundVelocity();
  JPH::Vec3 current_vertical_velocity = mCharacter->GetLinearVelocity().Dot(mCharacter->GetUp()) * mCharacter->GetUp();
  JPH::Vec3 ground_velocity = mCharacter->GetGroundVelocity();
  JPH::Vec3 new_velocity;
  bool moving_towards_ground = (current_vertical_velocity.GetY() - ground_velocity.GetY()) < 0.1f;
  if (mCharacter->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround &&
      (!mCharacter->IsSlopeTooSteep(mCharacter->GetGroundNormal()))) {
    new_velocity = ground_velocity;
    if (jump_triggered_ && moving_towards_ground) {
      new_velocity += 2 * jump_speed_  * mCharacter->GetUp();
      jump_triggered_ = false;
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


  // update for model mat at Render()
  // auto jph_pos = mCharacter->GetPosition();
  // position_ = glm::vec3(jph_pos.GetX(), jph_pos.GetY(), jph_pos.GetZ());
  // glm::quat base_rotation = glm::quat(rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ());
  // glm::quat visual_offset = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  // rotation_ = base_rotation * visual_offset;


  // --- animation walking
  auto velocity_mag = new_velocity.Length();
  bool is_moving = velocity_mag > 0.001f;
  Animation new_animationd_id;
  if (is_moving) {
    if (velocity_mag < .35f) {
      new_animationd_id = Animation::kWalk;
    } else {
      new_animationd_id = Animation::kRun;
    }
    // new_animationd_id = HumanAnimation::kCrouch;
  } else {
    new_animationd_id = Animation::kIdle;
  }

  if (IsIdle() && animation_id_ != new_animationd_id) {
    animation_id_ = new_animationd_id;
    animation_time_ = 0.0f;
    animation_looped_ = true;
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
      Jump(3.0f);
    } else if (key == GLFW_KEY_EQUAL) {
      Kick();
    } else if (key == GLFW_KEY_MINUS) {
      Stunned();
    } else if (key == GLFW_KEY_T) {
      Rest();
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

// should be called in Render(), it updates skin ubo
void Player::UpdateAnimation() {
  // bool started_over = animator_->UpdateUbo(animation_id_, animation_time_);
  // if (started_over && !animation_looped_) {
  //   ResetState();
  // }
}

void Player::Jump(float strength) {
    jump_triggered_ = true;
  return;
  if (IsIdle()) {
    // gravity_velocity_ = strength;
    state_ = State::kJumping;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kJump;
    animation_looped_ = false;
  }
}

/// --- states ---

void Player::Kick() {
  return;
  if (IsIdle()) {
    state_ = State::kAttacking;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kKick;
    animation_looped_ = false;
    next_event_ = Event(0.7f, PlayerEventType::DealDamage, false);
  }
}

void Player::Stunned() {
  return;
  if (IsIdle()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kStunned;
    animation_looped_ = false;
  }
}

void Player::Rest() {
  return;
  if (IsIdle()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kIdleSitting;
    animation_looped_ = false;
  }
}

void Player::Fall() {
  return;
  if (state_ != State::kFalling) {
    state_ = State::kFalling;
    animation_time_ = 0.0f;
    animation_id_ = Animation::kFall;
    animation_looped_ = true;
  }
}

void Player::ResetState() {
  animation_id_ = Animation::kIdle;
  animation_time_ = 0.0f;
  animation_looped_ = true;
  state_ = State::kIdle;
}

void Player::FireEvent(PlayerEventType e) {
  switch (e) {
    case PlayerEventType::DealDamage:
      DealDamage();
      break;
    case PlayerEventType::EnterIdle:
      EnterIdle();
      break;
    case PlayerEventType::EnterIdleSitting:
      EnterIdleSitting();
      break;
    default:
      break;
  }
}

void Player::DealDamage() {
  // AttackEvent attack;
  // attack.attacker = id_;
  // attack.damage = 1.0f;
  // attack.hitbox = GenWorldAabb();
  // attack_queue_->push_back(attack);
}

void Player::EnterIdle() {
  return;
  state_ = State::kIdle;
  animation_time_ = 0.0f;
  animation_id_ = Animation::kIdle;
  animation_looped_ = true;
}

void Player::EnterIdleSitting() {
  // state_ = State::kFpv;
  // animation_time_ = 0.0f;
  // animation_id_ = Animation::kIdleSitting;
  // animation_looped_ = true;
}
