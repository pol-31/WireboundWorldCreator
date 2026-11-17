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
    ray, hit, {}, {}, player_filter);

  // had_hit = physics_system_->GetNarrowPhaseQuery().CastRay(
  // ray, hit, JPH::SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::MOVING),
  // JPH::SpecifiedObjectLayerFilter(Layers::MOVING), player_filter);

  if (had_hit) {
    outPosition = ray.GetPointOnRay(hit.mFraction);
    outFraction = hit.mFraction;
    outID = hit.mBodyID;
  }


  if (had_hit) {
    // std::cout << "cast probe id : " << hit.mBodyID.GetIndex() << ' '
    // << std::boolalpha << had_hit << std::noboolalpha << std::endl;
  }
  return had_hit;
}

bool CharacterSharedData::CastProbeShoot(JPH::Vec3 pos, JPH::Vec3 dir,
  float inProbeLength, JPH::SubShapeID &outSubShapeId, float &outFraction,
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
    ray, hit, {}, {}, player_filter);

  if (had_hit) {
    outPosition = ray.GetPointOnRay(hit.mFraction);
    outFraction = hit.mFraction;
    outID = hit.mBodyID;
    outSubShapeId = hit.mSubShapeID2;
  }

  // had_hit = physics_system_->GetNarrowPhaseQuery().CastRay(
  // ray, hit, JPH::SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::MOVING),
  // JPH::SpecifiedObjectLayerFilter(Layers::MOVING), player_filter);

  if (had_hit) {
    // std::cout << "cast probe id : " << hit.mBodyID.GetIndex() << ' '
    // << std::boolalpha << had_hit << std::noboolalpha << std::endl;
  }
  return had_hit;
}

void CharacterSharedData::Shoot(JPH::Vec3 pos, JPH::Vec3 dir, JPH::BodyID source_body_id) {
  JPH::RVec3 hit_pos;
  JPH::BodyID hit_id;
  float hit_fraction = 1.0f;
  JPH::SubShapeID sub_shape_id;

  if (CastProbeShoot(pos, dir, 100.0f, sub_shape_id, hit_fraction, hit_pos, hit_id, source_body_id)) {
    JPH::Mat44 decal_matrix = JPH::Mat44::sIdentity();
    // 1. Extract world-space surface normal using Jolt's body lock
    JPH::Vec3 normal(0.0f, 1.0f, 0.0f);
    {
      JPH::BodyLockRead lock(physics_system_->GetBodyLockInterface(), hit_id);
      if (lock.Succeeded()) {
        const JPH::Body& body = lock.GetBody();
        normal = body.GetWorldSpaceSurfaceNormal(sub_shape_id, hit_pos);
      }
    }

    // 2. Nudge position slightly away from the wall to prevent Z-fighting artifacts
    JPH::Vec3 biased_position = hit_pos + (normal * 0.001f);

    // 3. Compute orthogonal Tangent and Bitangent vectors
    JPH::Vec3 world_up(0.0f, 1.0f, 0.0f);
    if (std::abs(normal.Dot(world_up)) > 0.99f) {
      world_up = JPH::Vec3(0.0f, 0.0f, 1.0f); // Safe fallback if hitting flat floor/ceiling
    }
    JPH::Vec3 tangent = world_up.Cross(normal).Normalized();
    JPH::Vec3 bitangent = normal.Cross(tangent).Normalized();

    // 4. Construct the JPH::Mat44 matrix
    // Columns map to: [0] = Tangent (X), [1] = Bitangent (Y), [2] = Normal (Z), [3] = Position (W)
    decal_matrix = JPH::Mat44(
      JPH::Vec4(tangent, 0.0f),
      JPH::Vec4(bitangent, 0.0f),
      JPH::Vec4(normal, 0.0f),
      JPH::Vec4(biased_position, 1.0f)
    );

    JPH::BodyInterface &bi = physics_system_->GetBodyInterface();
    JPH::Vec3 impulse = dir * 50.0f;
    bi.AddImpulse(hit_id, impulse, hit_pos);
    uint64_t user_data = bi.GetUserData(hit_id);
    if (user_data != 0) {
      GameObject* obj = reinterpret_cast<GameObject*>(user_data);
      obj->TakeDamage(impulse, hit_pos, 25.0f);
      auto obj_type = GetAssetDefaultProperties(obj->GetSceneNode()->mesh->name).type;
      if (obj_type == AssetProperties::Type::Static ||
        obj_type == AssetProperties::Type::Wall ||
        obj_type == AssetProperties::Type::Tile) {
        shoot_decals_.push_back({decal_matrix, 0});
      }
    }
  }
}
