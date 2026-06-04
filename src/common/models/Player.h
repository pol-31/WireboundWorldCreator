#ifndef WIREBOUNDWORLDCREATOR_PLAYER_H
#define WIREBOUNDWORLDCREATOR_PLAYER_H

#include <jolt/jolt.h>
#include <jolt/Physics/Character/CharacterVirtual.h>

#include "Scene.h"

class Animator;

class Player {
 public:
  enum class State {
    kIdle, // stand, walk, run
    kStunned // can't do anything
  };

  Player() = default;
  ~Player() = default;

  /// movement
  void ProcessMovement(int key, int action); // inside the glfwKeyCallback

  void SetMoveForward(bool pressed) { move_forward_ = pressed; }
  void SetMoveBackward(bool pressed) { move_backward_ = pressed; }
  void SetMoveLeft(bool pressed) { move_left_ = pressed; }
  void SetMoveRight(bool pressed) { move_right_ = pressed; }

  /// other

  void Render();

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

  void SetModel(const Scene::Model* model) {
    model_ = model;
  }

  void SetAnimator(Animator* animator);

  int GetAnimationId() const noexcept {
    return animation_id_;
  }

 private:
  const Scene::Model* model_ = nullptr;

  State state_ = State::kIdle;

  bool move_forward_ = false;
  bool move_backward_ = false;
  bool move_left_ = false;
  bool move_right_ = false;

  Animator* animator_;
  int animation_id_ = 0;

  float default_speed_ = 6.0f;
  float speed_ = default_speed_;
  float jump_speed_ = 4.0f;

  JPH::Ref<JPH::CharacterVirtual> mCharacter;
  bool mAllowSliding = false;
  bool jump_triggered_ = false;
};

#endif  // WIREBOUNDWORLDCREATOR_PLAYER_H
