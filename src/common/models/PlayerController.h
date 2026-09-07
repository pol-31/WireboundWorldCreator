#ifndef WIREBOUNDWORLDCREATOR_PLAYERCONTROLLER_H
#define WIREBOUNDWORLDCREATOR_PLAYERCONTROLLER_H

#include "Character.h"
#include "Scene.h"

class Camera;

class PlayerController {
public:
  PlayerController(
    const Camera* camera,
CharacterSharedData* shared_data,
SceneNode* scene_node,
const Scene::CharacterData* model,
const Scene::CharacterRig* skin);

  Character* GetBody() { return &character_; }
  const Character* GetBody() const { return &character_; }

  void ProcessMovement(int key, int action);

  void UpdateView();
  void UpdateMovementDirection();

  void SetAiming(bool value) {
    character_.is_aiming_ = value;
  }

  void Shoot();

  void TryEnterCover();
  void UpdateCoverState();
  void HandleCoverMovement(float inputX);
  void ExitCover();

  void ResetMouseDragging();
  void ReleaseObjectDragging(float throwForce);
  void UpdateObjectDragging();
  void StartObjectDragging();

  [[nodiscard]] bool IsDragging() const noexcept;

private:
  void SetMoveForward(bool pressed) { move_forward_ = pressed; }
  void SetMoveBackward(bool pressed) { move_backward_ = pressed; }
  void SetMoveLeft(bool pressed) { move_left_ = pressed; }
  void SetMoveRight(bool pressed) { move_right_ = pressed; }

  JPH::Vec3 GetMovementDirection();

  void UpdateRawMovementDirection();

  CharacterSharedData* GetSharedData() {
    return character_.shared_data_;
  }

  const Camera* camera_;

  bool move_forward_ = false;
  bool move_backward_ = false;
  bool move_left_ = false;
  bool move_right_ = false;

  Character character_;

  JPH::Body* mDragAnchor = nullptr;
  JPH::BodyID mDragBody = JPH::BodyID();
  JPH::Ref<JPH::Constraint> mDragConstraint = nullptr;
  JPH::uint mDragVertexIndex = ~JPH::uint(0);
  float mDragVertexPreviousInvMass = 0.0f;
  float mDragFraction = 0.0f;

  bool mIsInCover = false;
  JPH::BodyID mCoverBodyID;
  JPH::Vec3 mCoverNormal;
  JPH::Vec3 mCoverTangent;
  float mPeekOffset = 0.0f;
  const float cPlayerCoverRadius = 0.4f;
};

#endif  // WIREBOUNDWORLDCREATOR_PLAYERCONTROLLER_H
