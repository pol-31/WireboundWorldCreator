#include "CharacterSharedData.h"

#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

#include "../../core/Layers.h"

constexpr float cCharacterHeightStanding = 1.35f;
constexpr float cCharacterRadiusStanding = 0.3f;
constexpr float cCharacterHeightCrouching = 0.8f;
constexpr float cCharacterRadiusCrouching = 0.3f;
constexpr float cInnerShapeFraction = 0.9f;

JPH::Ref<JPH::CharacterVirtualSettings>
CharacterSharedData::GetDefaultJphSettings() {
  JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
  settings->mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);
  settings->mMaxStrength = 100.0f;
  settings->mShape = mStandingShape_;
  settings->mBackFaceMode = JPH::EBackFaceMode::CollideWithBackFaces;
  settings->mCharacterPadding = 0.02f;
  settings->mPenetrationRecoverySpeed = 1.0f;
  settings->mPredictiveContactDistance = 0.1f;
  settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -cCharacterRadiusStanding); // Accept contacts that touch the lower sphere of the capsule
  settings->mEnhancedInternalEdgeRemoval = false;
  settings->mInnerBodyShape = mInnerStandingShape_;
  settings->mInnerBodyLayer = Layers::MOVING;
  return settings;
}

CharacterSharedData::CharacterSharedData(
    const JPH::PhysicsSystem* physics_system,
    JPH::TempAllocator* temp_allocator,
    JPH::CharacterContactListener* contact_listener,
    Animator* animator)
      : physics_system_(physics_system),
  temp_allocator_(temp_allocator),
  contact_listener_(contact_listener),
  animator_(animator) {
  mStandingShape_ =
          JPH::RotatedTranslatedShapeSettings(
              JPH::Vec3(
                  0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding,
                  0),
              JPH::Quat::sIdentity(),
              new JPH::CapsuleShape(0.5f * cCharacterHeightStanding,
                                    cCharacterRadiusStanding))
              .Create()
              .Get();
  mCrouchingShape_ =
      JPH::RotatedTranslatedShapeSettings(
          JPH::Vec3(
              0,
              0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching,
              0),
          JPH::Quat::sIdentity(),
          new JPH::CapsuleShape(0.5f * cCharacterHeightCrouching,
                                cCharacterRadiusCrouching))
          .Create()
          .Get();
  mInnerStandingShape_ =
      JPH::RotatedTranslatedShapeSettings(
          JPH::Vec3(
              0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding,
              0),
          JPH::Quat::sIdentity(),
          new JPH::CapsuleShape(
              0.5f * cInnerShapeFraction * cCharacterHeightStanding,
              cInnerShapeFraction * cCharacterRadiusStanding))
          .Create()
          .Get();
  mInnerCrouchingShape_ =
      JPH::RotatedTranslatedShapeSettings(
          JPH::Vec3(
              0,
              0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching,
              0),
          JPH::Quat::sIdentity(),
          new JPH::CapsuleShape(
              0.5f * cInnerShapeFraction * cCharacterHeightCrouching,
              cInnerShapeFraction * cCharacterRadiusCrouching))
          .Create()
          .Get();

  //TODO: create shape here... isn't it already created above?
  // const auto* rt_shape = static_cast<const JPH::RotatedTranslatedShape*>(mInnerStandingShape_);
  // const JPH::Shape* core_shape = rt_shape->GetInnerShape();
  // const auto* capsule_shape = static_cast<const JPH::CapsuleShape*>(core_shape);
  // float diameter = capsule_shape->GetRadius();
  // JPH::Vec3 scale = JPH::Vec3(diameter, diameter, diameter);
  // matrix = matrix.PreScaled(scale);
}
