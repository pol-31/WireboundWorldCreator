#ifndef WIREBOUNDWORLDCREATOR_CHARACTERSHAREDDATA_H
#define WIREBOUNDWORLDCREATOR_CHARACTERSHAREDDATA_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include "Animator.h"

class CharacterSharedData {
public:
  CharacterSharedData(const JPH::PhysicsSystem* physics_system,
    JPH::TempAllocator* temp_allocator,
    JPH::CharacterContactListener* contact_listener,
    Animator* animator);

  JPH::Ref<JPH::CharacterVirtualSettings> GetDefaultJphSettings();

  /// set at Initialize()
  const JPH::PhysicsSystem* physics_system_;
  JPH::TempAllocator* temp_allocator_;
  JPH::CharacterContactListener* contact_listener_;
  Animator* animator_;

  JPH::CharacterVsCharacterCollisionSimple mCharacterVsCharacterCollision_;
  JPH::RefConst<JPH::Shape> mStandingShape_;
  JPH::RefConst<JPH::Shape> mCrouchingShape_;
  JPH::RefConst<JPH::Shape> mInnerCrouchingShape_;
  JPH::RefConst<JPH::Shape> mInnerStandingShape_;
};

#endif  // WIREBOUNDWORLDCREATOR_CHARACTERSHAREDDATA_H
