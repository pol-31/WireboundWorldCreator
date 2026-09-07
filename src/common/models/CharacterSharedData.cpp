#include "CharacterSharedData.h"

#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

#include "../../core/Layers.h"
#include "Animator.h"
#include "EnemyController.h"

#include <iostream>

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
    JPH::PhysicsSystem* physics_system,
    JPH::TempAllocator* temp_allocator,
    JPH::CharacterContactListener* contact_listener,
    Animator* animator,
    std::vector<std::unique_ptr<EnemyController>>* characters)
      : physics_system_(physics_system),
  temp_allocator_(temp_allocator),
  contact_listener_(contact_listener),
  animator_(animator),
  characters_(characters) {
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

bool CharacterSharedData::CastProbe(JPH::Vec3 pos, JPH::Vec3 dir,
  float inProbeLength, float &outFraction,
    JPH::RVec3 &outPosition, JPH::BodyID &outID, JPH::BodyID source_body_id) {
  JPH::RVec3 start = pos;
  JPH::Vec3 direction = inProbeLength * dir;

  // Clear output
  outPosition = start + direction;
  outFraction = 1.0f;
  outID = JPH::BodyID();

  bool had_hit = false;
  JPH::RRayCast ray{start, direction};
  JPH::RayCastResult hit;
  IgnoreSingleBodyFilter player_filter = IgnoreSingleBodyFilter(source_body_id);
  had_hit = physics_system_->GetNarrowPhaseQuery().CastRay(
    ray, hit, JPH::SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::MOVING),
    JPH::SpecifiedObjectLayerFilter(Layers::MOVING), player_filter);

  outPosition = ray.GetPointOnRay(hit.mFraction);
  outFraction = hit.mFraction;
  outID = hit.mBodyID;

  // if (had_hit)
  //   mDebugRenderer->DrawMarker(outPosition, JPH::Color::sYellow, 0.1f);
  // else
  //   mDebugRenderer->DrawMarker(pos + 0.1f * forward, JPH::Color::sRed, 0.001f);

  if (had_hit) {
    std::cout << "cast probe id : " << hit.mBodyID.GetIndex() << ' '
    << std::boolalpha << had_hit << std::noboolalpha << std::endl;
  }
  return had_hit;
}

void CharacterSharedData::Shoot(JPH::Vec3 pos, JPH::Vec3 dir, JPH::BodyID source_body_id) {
  JPH::RVec3 hit_position;
  JPH::BodyID hit_body_id;
  float hit_fraction = 1.0f;
  float maxDistance = 100.0f;
  if (CastProbe(pos, dir, maxDistance, hit_fraction,
    hit_position, hit_body_id, source_body_id)) {
    float shotForce = 50.0f;
    JPH::Vec3 impulse = dir * shotForce;
    JPH::BodyInterface &bi = physics_system_->GetBodyInterface();
    bool is_character = false;
    //TODO: probably there's a better way
    for (auto& c : *characters_) {
      if (c->GetBody()->GetJphCharacter()->GetInnerBodyID() == hit_body_id) {
        is_character = true;
        float stopping_power = 15.0f;
        c->GetBody()->Death();
        std::cout << "shot somebody" << std::endl;
        // c.external_impulse += ToJph(camera_.GetDirectionFront()) * stopping_power;
        return;
      }
    }
      bi.AddImpulse(hit_body_id, impulse, hit_position);
      std::cout << "shot something" << std::endl;
  }
}
