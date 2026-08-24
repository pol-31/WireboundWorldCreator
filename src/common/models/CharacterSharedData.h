#ifndef WIREBOUNDWORLDCREATOR_CHARACTERSHAREDDATA_H
#define WIREBOUNDWORLDCREATOR_CHARACTERSHAREDDATA_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

class Animator;
class EnemyController;

class CharacterSharedData {
public:
  CharacterSharedData(JPH::PhysicsSystem* physics_system,
    JPH::TempAllocator* temp_allocator,
    JPH::CharacterContactListener* contact_listener,
    Animator* animator,
    std::vector<std::unique_ptr<EnemyController>>* characters);

  JPH::Ref<JPH::CharacterVirtualSettings> GetDefaultJphSettings();

  bool CastProbe(JPH::Vec3 pos, JPH::Vec3 dir, float inProbeLength, float &outFraction,
    JPH::RVec3 &outPosition, JPH::BodyID &outID, JPH::BodyID source_body_id);

  void Shoot(JPH::Vec3 pos, JPH::Vec3 dir, JPH::BodyID source_body_id);

  /// set at Initialize()
  JPH::PhysicsSystem* physics_system_;
  JPH::TempAllocator* temp_allocator_;
  JPH::CharacterContactListener* contact_listener_;
  Animator* animator_;

  JPH::CharacterVsCharacterCollisionSimple mCharacterVsCharacterCollision_;
  JPH::RefConst<JPH::Shape> mStandingShape_;
  JPH::RefConst<JPH::Shape> mCrouchingShape_;
  JPH::RefConst<JPH::Shape> mInnerCrouchingShape_;
  JPH::RefConst<JPH::Shape> mInnerStandingShape_;

  std::vector<std::unique_ptr<EnemyController>>* characters_;
};

class IgnoreSingleBodyFilter : public JPH::BodyFilter {
public:
  IgnoreSingleBodyFilter(const JPH::BodyID& inIgnoreMe) : mIgnoreMe(inIgnoreMe) {}
  virtual bool ShouldCollide(const JPH::BodyID& inBodyID) const override {
    return inBodyID != mIgnoreMe; // Skip the player
  }
private:
  JPH::BodyID mIgnoreMe;
};

#endif  // WIREBOUNDWORLDCREATOR_CHARACTERSHAREDDATA_H
