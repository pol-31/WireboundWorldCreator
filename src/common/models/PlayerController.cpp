#include "PlayerController.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "../../core/Layers.h"
#include "../../io/Window.h" // gDeltaTime
#include "../../io/Camera.h"

const float cDragRayLength = 40.0f;

PlayerController::PlayerController(
    const Camera* camera,
CharacterSharedData* shared_data,
SceneNode* scene_node,
const Scene::CharacterData* model,
const Scene::CharacterRig* skin)
: character_(shared_data, scene_node, model, skin),
  camera_(camera) {
  character_.jph_character_->SetCharacterVsCharacterCollision(&shared_data->mCharacterVsCharacterCollision_);
  character_.jph_character_->SetListener(shared_data->contact_listener_);
  ResetMouseDragging();
}

void PlayerController::UpdateView() {
  float yaw = camera_->GetYaw();
  float pitch = camera_->GetPitch();
  if (!camera_->IsFirstFaceMode()) {
    // to look straight at the camera it's +90, now that's the opposite
    yaw = std::remainder(-camera_->GetYaw() - 90.0f, 360.0f);
  }

  UpdateRawMovementDirection();
  auto input_dir = character_.GetMoveDirectionRaw();
  character_.head_yaw_ = -yaw;
  character_.head_pitch_ = pitch;
  JPH::Quat rotation = JPH::Quat::sRotation(JPH::Vec3::sAxisY(), -JPH::DegreesToRadians(yaw));
  character_.movement_direction_ = rotation * input_dir;
}

void PlayerController::UpdateRawMovementDirection() {
  auto& dir = character_.movement_direction_raw_;
  dir = JPH::Vec3::sZero();
  if (move_left_) dir.SetZ(-1);
  if (move_right_) dir.SetZ(1);
  if (move_forward_) dir.SetX(1);
  if (move_backward_) dir.SetX(-1);
  if (dir != JPH::Vec3::sZero())
    dir = dir.Normalized();
}

void PlayerController::ProcessMovement(int key, int action) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_W) {
      SetMoveForward(true);
    } else if (key == GLFW_KEY_S) {
      SetMoveBackward(true);
    } else if (key == GLFW_KEY_A) {
      SetMoveLeft(true);
    } else if (key == GLFW_KEY_D) {
      SetMoveRight(true);
    } else if (key == GLFW_KEY_R) {
      character_.Reload();
    } else if (key == GLFW_KEY_SPACE) {
      character_.jump_triggered_ = true;
    } else if (key == GLFW_KEY_EQUAL) {
      // Kick();
    } else if (key == GLFW_KEY_MINUS) {
      // Stunned();
    } else if (key == GLFW_KEY_T) {
      // Rest();
    } else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
      character_.Run();
    } else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
      character_.Crouch();
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W) {
      SetMoveForward(false);
    } else if (key == GLFW_KEY_S) {
      SetMoveBackward(false);
    } else if (key == GLFW_KEY_A) {
      SetMoveLeft(false);
    } else if (key == GLFW_KEY_D) {
      SetMoveRight(false);
    } else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
      character_.BackToWalk();
    } else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
      character_.BackToWalk();
    }
  }
}

void PlayerController::TryEnterCover() {
    float coverCheckDistance = 1.5f;
    JPH::RefConst<JPH::Shape> playerShape = new JPH::SphereShape(cPlayerCoverRadius * 2.0f);

    // 1. Create the correct RShapeCast object
    // Pass local space shape, scale, world transform matrix, and translation direction
    JPH::RShapeCast shapeCast(
        playerShape,
        JPH::Vec3::sReplicate(1.0f),
        JPH::Mat44::sTranslation(ToJph(camera_->GetPosition())),
        ToJph(camera_->GetDirectionFront()) * coverCheckDistance
    );

    JPH::ShapeCastSettings castSettings;
    castSettings.mReturnDeepestPoint = true; // Ensures accurate penetration data for snapping

  JPH::ClosestHitCollisionCollector<JPH::CastShapeCollector> collector;

  IgnoreSingleBodyFilter player_filter = IgnoreSingleBodyFilter(
    character_.GetJphCharacter()->GetInnerBodyID());

    // 3. Call the function with all required filters and offsets
    GetSharedData()->physics_system_->GetNarrowPhaseQuery().CastShape(
        shapeCast,
        castSettings,
        JPH::RVec3::sZero(), // inBaseOffset (can be zero since our shapeCast matrix is already in world space)
        collector,
        {}, {}, player_filter);

    // 4. Check if the collector actually found a hit
    if (collector.HadHit()) {
        const JPH::ShapeCastResult& hit = collector.mHit;
        JPH::BodyInterface& bi = GetSharedData()->physics_system_->GetBodyInterface();

        // Double check it's static geometry
        if (bi.GetMotionType(hit.mBodyID2) == JPH::EMotionType::Static) {
            mIsInCover = true;
            mCoverBodyID = hit.mBodyID2;

            // In Jolt, mPenetrationAxis points from shape2 to shape1
            // Normalized, this gives us the wall normal pointing outward
            mCoverNormal = hit.mPenetrationAxis.Normalized();
            mCoverTangent = JPH::Vec3::sAxisY().Cross(mCoverNormal).Normalized();

            // Calculate snap position using the hit fraction along the cast path
            JPH::Vec3 hitPosition = shapeCast.GetPointOnRay(hit.mFraction);
            JPH::Vec3 targetSnapPosition = hitPosition + (mCoverNormal * cPlayerCoverRadius);

          character_.GetJphCharacter()->SetPosition(targetSnapPosition);

          std::cout << mCoverBodyID.GetIndex() << ' '
          << targetSnapPosition.GetX() << ' ' << targetSnapPosition.GetZ()
          << std::endl;
            // Move player to targetSnapPosition...
        }
    }
}

void PlayerController::UpdateCoverState() {
  if (!mIsInCover) return;

  JPH::BodyInterface& bi = GetSharedData()->physics_system_->GetBodyInterface();

  // 1. Check if the body was deleted entirely (e.g., deleted from the physics world)
  // 2. Check if the static board was converted into a dynamic flying chunk
  if (!bi.IsAdded(mCoverBodyID) || bi.GetMotionType(mCoverBodyID) != JPH::EMotionType::Static) {

    // The cover has been blown apart!
    mIsInCover = false;
    mCoverBodyID = JPH::BodyID();

    // Optional: Spike the Sanity/Stress meter here because the player
    // just had their cover shot out from under them.
    // IncreaseSanityStress(25.0f);
  }
}

void PlayerController::HandleCoverMovement(float inputX) {
  if (!mIsInCover) return;

  // --- SLIDING ---
  // inputX is -1.0f (Left/A) or 1.0f (Right/D)
  float slideSpeed = 2.0f;
  JPH::Vec3 velocity = mCoverTangent * (inputX * slideSpeed);

  // Apply this velocity to your player controller
  // mPlayerCharacter->SetLinearVelocity(velocity);

  // --- PEEKING ---
  // If the player holds LMB, we smoothly interpolate the camera sideways
  float maxPeekDistance = 0.6f;
  float peekSpeed = 5.0f;

  if (character_.IsAiming()) {
    // Slide camera outward based on which direction we are pressing
    // If holding D (right), peek right. If holding A (left), peek left.
    float targetPeek = inputX * maxPeekDistance;

    // If not pressing A or D, just peek right by default
    if (inputX == 0.0f) targetPeek = maxPeekDistance;

    // Smooth lerp
    mPeekOffset += (targetPeek - mPeekOffset) * peekSpeed * gDeltaTime;

    // FIRE LOGIC: Your hitscan from the previous step goes here,
    // using the new offset camera position.
  } else {
    // Smoothly return behind cover when not shooting
    mPeekOffset += (0.0f - mPeekOffset) * peekSpeed * gDeltaTime;
  }

  // Apply the visual peek offset to your actual OpenGL camera rendering position
  // Vec3 finalCameraRenderPos = playerBasePos + (mCoverTangent * mPeekOffset);
  // camera_.SetPosition(finalCameraRenderPos);
}

void PlayerController::ExitCover() {
  mIsInCover = false;
}

void PlayerController::ResetMouseDragging() {
  mDragAnchor = nullptr;
  mDragBody = JPH::BodyID();
  mDragConstraint = nullptr;
  mDragVertexIndex = ~JPH::uint(0);
  mDragVertexPreviousInvMass = 0.0f;
  mDragFraction = 0.0f;
}

void PlayerController::ReleaseObjectDragging(float throwForce) {
  if (!IsDragging()) {
    return;
  }
  JPH::BodyInterface &bi = GetSharedData()->physics_system_->GetBodyInterface();
  JPH::Vec3 throwVector = ToJph(camera_->GetDirectionFront()) * throwForce;
  if (mDragConstraint != nullptr) {
    GetSharedData()->physics_system_->RemoveConstraint(mDragConstraint);
    mDragConstraint = nullptr;
    if (throwForce != 0.0f) {
      bi.AddImpulse(mDragBody, throwVector);
    }
  }
  if (mDragAnchor != nullptr) {
    bi.DestroyBody(mDragAnchor->GetID());
    mDragAnchor = nullptr;
  }
  if (mDragVertexIndex != ~JPH::uint(0)) {
    JPH::BodyLockWrite lock(GetSharedData()->physics_system_->GetBodyLockInterface(),
                       mDragBody);
    if (lock.Succeeded()) {
      JPH::Body &body = lock.GetBody();
      JPH_ASSERT(body.IsSoftBody());
      JPH::SoftBodyMotionProperties *mp =
          static_cast<JPH::SoftBodyMotionProperties *>(
              body.GetMotionProperties());
      mp->GetVertex(mDragVertexIndex).mInvMass =
          mDragVertexPreviousInvMass;
      if (throwForce != 0.0f) {
        for (JPH::SoftBodyVertex& vertex : mp->GetVertices()) {
          if (vertex.mInvMass > 0.0f) { // Don't accelerate fixed/kinematic vertices
            vertex.mVelocity += throwVector;
          }
        }
      }
    }
    mDragVertexIndex = ~JPH::uint(0);
    mDragVertexPreviousInvMass = 0;
  }
  mDragBody = JPH::BodyID();
}

void PlayerController::UpdateObjectDragging() {
  if (!IsDragging()) {
    return;
  }
  JPH::BodyInterface &bi = GetSharedData()->physics_system_->GetBodyInterface();
  JPH::RVec3 new_pos = ToJph(camera_->GetPosition()) +
    cDragRayLength * mDragFraction * ToJph(camera_->GetDirectionFront());
  switch (bi.GetBodyType(mDragBody)) {
    case JPH::EBodyType::RigidBody:
      bi.SetPositionAndRotation(mDragAnchor->GetID(), new_pos,
                                JPH::Quat::sIdentity(),
                                JPH::EActivation::DontActivate);
      break;
    case JPH::EBodyType::SoftBody: {
      JPH::BodyLockWrite lock(GetSharedData()->physics_system_->GetBodyLockInterface(),
                         mDragBody);
      if (lock.Succeeded()) {
        JPH::Body &body = lock.GetBody();
        JPH::SoftBodyMotionProperties *mp =
            static_cast<JPH::SoftBodyMotionProperties *>(
                body.GetMotionProperties());
        JPH::SoftBodyVertex &v = mp->GetVertex(mDragVertexIndex);
        v.mVelocity = body.GetRotation().Conjugated() *
                      JPH::Vec3(new_pos - body.GetCenterOfMassTransform() *
                                         v.mPosition) /
                      gDeltaTimePhysics;
      }
    } break;
  }
  bi.ActivateBody(mDragBody);
}

void PlayerController::StartObjectDragging() {
  if (IsDragging()) {
    return;
  }
  JPH::BodyInterface &bi = GetSharedData()->physics_system_->GetBodyInterface();
  JPH::RVec3 hit_position;
  if (GetSharedData()->CastProbe(
    ToJph(camera_->GetPosition()), ToJph(camera_->GetDirectionFront()),
    cDragRayLength, mDragFraction, hit_position,
    mDragBody, character_.jph_character_->GetInnerBodyID())) {
    JPH::BodyLockWrite lock(GetSharedData()->physics_system_->GetBodyLockInterface(),
                               mDragBody);
    if (lock.Succeeded()) {
      JPH::Body &drag_body = lock.GetBody();
      if (drag_body.IsSoftBody()) {
        JPH::SoftBodyMotionProperties *mp =
            static_cast<JPH::SoftBodyMotionProperties *>(
                drag_body.GetMotionProperties());

        JPH::Vec3 local_hit_position = JPH::Vec3(
            drag_body.GetInverseCenterOfMassTransform() * hit_position);
        float closest_dist_sq = FLT_MAX;
        for (JPH::SoftBodyVertex &v : mp->GetVertices()) {
          float dist_sq = (v.mPosition - local_hit_position).LengthSq();
          if (dist_sq < closest_dist_sq) {
            closest_dist_sq = dist_sq;
            mDragVertexIndex = JPH::uint(&v - mp->GetVertices().data());
          }
        }

        JPH::SoftBodyVertex &v = mp->GetVertex(mDragVertexIndex);
        mDragVertexPreviousInvMass = v.mInvMass;
        v.mInvMass = 0.0f;
      } else if (drag_body.IsDynamic()) {
        JPH::DistanceConstraintSettings settings;
        settings.mPoint1 = settings.mPoint2 = hit_position;
        settings.mLimitsSpringSettings.mFrequency = 2.0f; // div by world_scale==1
        settings.mLimitsSpringSettings.mDamping = 1.0f;

        JPH::Body *drag_anchor = bi.CreateBody(JPH::BodyCreationSettings(
            new JPH::SphereShape(0.01f), hit_position, JPH::Quat::sIdentity(),
            JPH::EMotionType::Static, Layers::NON_MOVING));
        mDragAnchor = drag_anchor;

        mDragConstraint = settings.Create(*drag_anchor, drag_body);
        GetSharedData()->physics_system_->AddConstraint(mDragConstraint);
      }
    }
  }
}

bool PlayerController::IsDragging() const noexcept {
  return mDragConstraint != nullptr || mDragVertexIndex != ~JPH::uint(0);
}

void PlayerController::Shoot() {
  character_.Shoot(ToJph(camera_->GetPosition()),
    ToJph(camera_->GetDirectionFront()));
}
