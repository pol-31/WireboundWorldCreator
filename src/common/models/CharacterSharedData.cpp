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
    ray, hit, {}, {}, player_filter);
  outPosition = ray.GetPointOnRay(hit.mFraction);
  outFraction = hit.mFraction;
  outID = hit.mBodyID;

  /*
   bool had_hit = false;
JPH::RRayCast ray{start, direction};
JPH::RayCastResult hit;
IgnoreSingleBodyFilter player_filter = IgnoreSingleBodyFilter(source_body_id);

had_hit = physics_system_->GetNarrowPhaseQuery().CastRay(
  ray, hit, {}, {}, player_filter);

JPH::Mat44 decal_matrix = JPH::Mat44::sIdentity();

if (had_hit) {
  JPH::Vec3 outPosition = ray.GetPointOnRay(hit.mFraction);
  float outFraction = hit.mFraction;
  JPH::BodyID outID = hit.mBodyID;

  // 1. Extract world-space surface normal using Jolt's body lock
  JPH::Vec3 normal(0.0f, 1.0f, 0.0f);
  {
    JPH::BodyLockRead lock(physics_system_->GetBodyLockInterface(), outID);
    if (lock.Succeeded()) {
      const JPH::Body& body = lock.GetBody();
      normal = body.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, outPosition);
    }
  }

  // 2. Nudge position slightly away from the wall to prevent Z-fighting artifacts
  JPH::Vec3 biased_position = outPosition + (normal * 0.002f);

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
}
   *
   */

  // had_hit = physics_system_->GetNarrowPhaseQuery().CastRay(
  // ray, hit, JPH::SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::MOVING),
  // JPH::SpecifiedObjectLayerFilter(Layers::MOVING), player_filter);

  // if (had_hit)
  //   mDebugRenderer->DrawMarker(outPosition, JPH::Color::sYellow, 0.1f);
  // else
  //   mDebugRenderer->DrawMarker(pos + 0.1f * forward, JPH::Color::sRed, 0.001f);

  if (had_hit) {
    // std::cout << "cast probe id : " << hit.mBodyID.GetIndex() << ' '
    // << std::boolalpha << had_hit << std::noboolalpha << std::endl;
  }
  return had_hit;
}

struct ShootDecalInfo {
  JPH::Vec3 position;
  JPH::Vec3 normal;
  float roll_angle = 0.0f; // Adds visual variety
  int atlas_index = 0;
};

class DecalManager {
private:
  static constexpr size_t kMaxDecals = 20;
  std::array<ShootDecalInfo, kMaxDecals> decals_;
  size_t head_ = 0;
  size_t count_ = 0;

public:
  void AddDecal(const ShootDecalInfo& decal) {
    decals_[head_] = decal;
    head_ = (head_ + 1) % kMaxDecals; // Overwrites oldest on overflow
    if (count_ < kMaxDecals) {
      count_++;
    }
  }

  size_t GetCount() const { return count_; }
  const ShootDecalInfo& GetDecal(size_t index) const { return decals_[index]; }
};

// void CharacterSharedData::Shoot(JPH::Vec3 pos, JPH::Vec3 dir, JPH::BodyID source_body_id) {
//   JPH::RVec3 hit_position;
//   JPH::BodyID hit_body_id;
//   float hit_fraction = 1.0f;
//   float maxDistance = 100.0f;
//   if (CastProbe(pos, dir, maxDistance, hit_fraction,
//     hit_position, hit_body_id, source_body_id)) {
//     float shotForce = 50.0f;
//     JPH::Vec3 impulse = dir * shotForce;
//     JPH::BodyInterface &bi = physics_system_->GetBodyInterface();
//     bool is_character = false;
//     //TODO: probably there's a better way
//     for (auto& c : *characters_) {
//       if (c->GetBody()->GetJphCharacter()->GetInnerBodyID() == hit_body_id) {
//         is_character = true;
//         float stopping_power = 15.0f;
//         c->GetBody()->Death();
//         std::cout << "shot somebody" << std::endl;
//         // c.external_impulse += ToJph(camera_.GetDirectionFront()) * stopping_power;
//         return;
//       }
//     }
//     for (auto& d : *doors_) {
//       d.TakeDamage(hit_body_id, impulse, 100.0f);
//     }
//       bi.AddImpulse(hit_body_id, impulse, hit_position);
//       std::cout << "shot something" << std::endl;
//   }
// }

void CharacterSharedData::Shoot(JPH::Vec3 pos, JPH::Vec3 dir, JPH::BodyID source_body_id) {
  JPH::RVec3 hit_pos;
  JPH::BodyID hit_id;
  float hit_fraction = 1.0f;

  if (CastProbe(pos, dir, 100.0f, hit_fraction, hit_pos, hit_id, source_body_id)) {
    JPH::BodyInterface &bi = physics_system_->GetBodyInterface();
    JPH::Vec3 impulse = dir * 50.0f;
    bi.AddImpulse(hit_id, impulse, hit_pos);
    uint64_t user_data = bi.GetUserData(hit_id);
    if (user_data != 0) {
      GameObject* obj = reinterpret_cast<GameObject*>(user_data);
      obj->TakeDamage(impulse, hit_pos, 25.0f);
    }
  }
}
