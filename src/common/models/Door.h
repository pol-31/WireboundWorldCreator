#ifndef WIREBOUNDWORLDCREATOR_DOOR_H
#define WIREBOUNDWORLDCREATOR_DOOR_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "GameObject.h"
#include "Scene.h"
#include "miniaudio.h"

class Scene;
class Hinge;

class Door : public GameObject {
 public:
  Door() = default;

  void AddHinge(Hinge* hinge) {
    hinges_.push_back(hinge);
  }

  void Initialize(JPH::PhysicsSystem* physics_system,
       SceneNode* desk, JPH::BodyID frame_body_id,
       ma_engine* audio_engine, ma_sound_group* sfx_group);

  InteractPrompt GetInteractPrompt(Character* interactor) override;

  void Interact(Character* interactor) override;

  JPH::Vec3 GetPosition() const override;


    void Update(JPH::BodyInterface* bi, float deltaTime);

 private:
  JPH::PhysicsSystem* physics_system_ = nullptr;
  std::vector<Hinge*> hinges_;
  ma_sound sound_creak_;
};

#endif  // WIREBOUNDWORLDCREATOR_DOOR_H
