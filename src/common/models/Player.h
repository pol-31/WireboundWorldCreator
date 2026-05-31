#ifndef WIREBOUNDWORLDCREATOR_PLAYER_H
#define WIREBOUNDWORLDCREATOR_PLAYER_H

#include <jolt/jolt.h>
#include <jolt/Physics/Character/CharacterVirtual.h>

#include "Scene.h"
#include "Animator.h"

class Player {
 public:
  enum class State { kIdle, kJumping, kFalling, kAttacking, kStunned };

  enum class PlayerEventType {
    None,
    DealDamage,
    OpenUI,
    EnterIdle,
    EnterIdleSitting,
  };

  Player();

  ~Player();

  void UpdateUbo();

  void Jump(float strength);

  void Kick();

  void Rest();

  void Stunned();

  void Fall();  // internally called by ApplyGravity()

  void ResetState();

  void FireEvent(PlayerEventType e);

  void DealDamage();

  void EnterIdle();

  void EnterIdleSitting();

  struct Event {
    float time = 0.f;
    PlayerEventType type = PlayerEventType::None;
    bool done = true;
  };

  Event next_event_;

  [[nodiscard]] bool IsOnGround() const noexcept {
    return state_ != State::kJumping && state_ != State::kFalling;
  }

  [[nodiscard]] bool IsIdle() const noexcept { return state_ == State::kIdle; }

  /// movement

  void ProcessMovement(int key, int action); // inside the glfwKeyCallback

  void SetMoveForward(bool pressed) { move_forward_ = pressed; }
  void SetMoveBackward(bool pressed) { move_backward_ = pressed; }
  void SetMoveLeft(bool pressed) { move_left_ = pressed; }
  void SetMoveRight(bool pressed) { move_right_ = pressed; }

  /// other

  // should be called in Render(), it updates skin ssbo
  void UpdateAnimation();

  void Render(float map_scale);

  // void SetAnimator(Animator* animator) { animator_ = animator; }

  bool switch_stance_triggered_ = false;

  void SetCharacterVirtual(JPH::CharacterVirtual* character,
    JPH::CharacterVsCharacterCollision* mCharacterVsCharacterCollision,
    JPH::CharacterContactListener* contact_listener) {
    mCharacter = character;
    mCharacter->SetCharacterVsCharacterCollision(mCharacterVsCharacterCollision);
    mCharacter->SetListener(contact_listener);
  }
  JPH::CharacterVirtual* GetJphCharacter() {
    return mCharacter;
  }

  void Update(JPH::Vec3 camera_forward, JPH::Vec3 gravity);

  void AllowSliding(bool val) {
    mAllowSliding = val;
  }

  bool GetAllowSliding() const {
    return mAllowSliding;
  }

  [[nodiscard]] const Scene::Model* GetModel() const noexcept {
    return model_;
  }
  void SetModel(Scene::Model* model) {
    model_ = model;
  }

 private:
  Scene::Model* model_ = nullptr;

  State state_ = State::kIdle;

  bool move_forward_ = false;
  bool move_backward_ = false;
  bool move_left_ = false;
  bool move_right_ = false;

  float animation_time_ = 0.0f;
  bool animation_looped_ = true;
  // Animator* animator_ = nullptr;
  Animation animation_id_ = Animation::kIdle;  // todo; merge with state_?

  float default_speed_ = 6.0f;
  float speed_ = default_speed_;
  float jump_speed_ = 4.0f;

  JPH::Ref<JPH::CharacterVirtual> mCharacter;
  bool mAllowSliding = false;
  bool jump_triggered_ = false;

  GLuint ubo_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_PLAYER_H
