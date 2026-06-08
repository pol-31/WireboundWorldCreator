#include "CharacterBaseTest.h"
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>
#include <Jolt/ObjectStream/ObjectStreamIn.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>

#include "Layers.h"
#include "../io/Window.h"
// #include <Utils/Log.h>
// #include <Utils/AssetStream.h>

// Scene constants
static const JPH::RVec3 cRotatingPosition(-5, 0.15f, 15);
static const JPH::Quat cRotatingOrientation = JPH::Quat::sIdentity();
static const JPH::RVec3 cRotatingWallPosition(5, 1.0f, 25.0f);
static const JPH::Quat cRotatingWallOrientation = JPH::Quat::sIdentity();
static const JPH::RVec3 cRotatingAndTranslatingPosition(-10, 0.15f, 27.5f);
static const JPH::Quat cRotatingAndTranslatingOrientation =
    JPH::Quat::sIdentity();
static const JPH::RVec3 cSmoothVerticallyMovingPosition(0, 2.0f, 15);
static const JPH::Quat cSmoothVerticallyMovingOrientation =
    JPH::Quat::sIdentity();
static const JPH::RVec3 cReversingVerticallyMovingPosition(0, 0.15f, 25);
static const JPH::Quat cReversingVerticallyMovingOrientation =
    JPH::Quat::sIdentity();
static const JPH::RVec3 cHorizontallyMovingPosition(5, 1, 15);
static const JPH::Quat cHorizontallyMovingOrientation =
    JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), 0.5f * JPH::JPH_PI);
static const JPH::RVec3 cConveyorBeltPosition(-10, 0.15f, 15);
static const JPH::RVec3 cRampPosition(15, 2.2f, 15);
static const JPH::Quat cRampOrientation =
    JPH::Quat::sRotation(JPH::Vec3::sAxisX(), -0.25f * JPH::JPH_PI);
static const JPH::RVec3 cRampBlocksStart =
    cRampPosition + JPH::Vec3(-3.0f, 3.0f, 1.5f);
static const JPH::Vec3 cRampBlocksDelta = JPH::Vec3(2.0f, 0, 0);
static const float cRampBlocksTime = 5.0f;
static const JPH::RVec3 cSmallBumpsPosition(-5.0f, 0, 2.5f);
static const float cSmallBumpHeight = 0.05f;
static const float cSmallBumpWidth = 0.01f;
static const float cSmallBumpDelta = 0.5f;
static const JPH::RVec3 cLargeBumpsPosition(-10.0f, 0, 2.5f);
static const float cLargeBumpHeight = 0.3f;
static const float cLargeBumpWidth = 0.1f;
static const float cLargeBumpDelta = 2.0f;
static const JPH::RVec3 cStairsPosition(-15.0f, 0, 2.5f);
static const float cStairsStepHeight = 0.3f;
static const JPH::RVec3 cMeshStairsPosition(-20.0f, 0, 2.5f);
static const JPH::RVec3 cNoStairsPosition(-15.0f, 0, 10.0f);
static const float cNoStairsStepHeight = 0.3f;
static const float cNoStairsStepDelta = 0.05f;
static const JPH::RVec3 cMeshNoStairsPosition(-20.0f, 0, 10.0f);
static const JPH::RVec3 cMeshWallPosition(-25.0f, 0, -27.0f);
static const float cMeshWallHeight = 3.0f;
static const float cMeshWallWidth = 2.0f;
static const float cMeshWallStepStart = 0.5f;
static const float cMeshWallStepEnd = 4.0f;
static const int cMeshWallSegments = 25;
static const JPH::RVec3 cHalfCylinderPosition(5.0f, 0, 8.0f);
static const JPH::RVec3 cMeshBoxPosition(30.0f, 1.5f, 5.0f);
static const JPH::RVec3 cSensorPosition(30, 0.9f, -5);
static const JPH::RVec3 cCharacterPosition(-3.5f, 0, 3.0f);
static const JPH::RVec3 cCharacterVirtualPosition(-5.0f, 0, 3.0f);
static const JPH::RVec3 cCharacterVirtualWithInnerBodyPosition(-6.5f, 0, 3.0f);
static const JPH::Vec3 cCharacterVelocity(0, 0, 2);

constexpr float cCharacterHeightStanding = 1.35f;
constexpr float cCharacterRadiusStanding = 0.3f;
constexpr float cCharacterHeightCrouching = 0.8f;
constexpr float cCharacterRadiusCrouching = 0.3f;
constexpr float cInnerShapeFraction = 0.9f;

static const float cCollisionTolerance = 0.05f;

CharacterBaseTest::CharacterBaseTest(
    JPH::PhysicsSystem *inPhysicsSystem,
    JPH::JobSystem *inJobSystem,
    JPH::TempAllocator *inTempAllocator,
    Renderer *inDebugRenderer)
    : mPhysicsSystem(inPhysicsSystem),
      mBodyInterface(&inPhysicsSystem->GetBodyInterface()),
      mJobSystem(inJobSystem),
      mTempAllocator(inTempAllocator),
      renderer_(inDebugRenderer),
      player_() {
  Init();
}

void CharacterBaseTest::DeInit() {
  // mCharacter->RemoveFromPhysicsSystem();
}

void CharacterBaseTest::CreateCharacterShapes() {
  mStandingShape =
          JPH::RotatedTranslatedShapeSettings(
              JPH::Vec3(
                  0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding,
                  0),
              JPH::Quat::sIdentity(),
              new JPH::CapsuleShape(0.5f * cCharacterHeightStanding,
                                    cCharacterRadiusStanding))
              .Create()
              .Get();
  mCrouchingShape =
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
  mInnerStandingShape =
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
  mInnerCrouchingShape =
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

void CharacterBaseTest::CreateCollisionShapes() {
  mShapeToGeometry[JPH::EShapeSubType::Box]  = 0; // box
  mShapeToGeometry[JPH::EShapeSubType::Sphere]  = 1; // isosphere
  mShapeToGeometry[JPH::EShapeSubType::Plane] = 2; // capsule
  mShapeToGeometry[JPH::EShapeSubType::Capsule] = 3; // capsule
}

void CharacterBaseTest::AddShotShere(JPH::BodyID body_id) {
  shot_objects_.push_back(body_id);
}

void CharacterBaseTest::Init() {
  // auto mdl_tree =
      // mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\MapMarker.gltf", 1);
  // auto mdl_human =
      // mdl_loader_.LoadScene("C:\\Users\\Pavlo\\Desktop\\assets\\Human.gltf", 2);
  mdl_loader_.LoadScene(
        "C:\\Users\\Pavlo\\Desktop\\assets\\CollisionShapes1.gltf",
        "C:\\Users\\Pavlo\\Desktop\\assets\\Human1.gltf",
        "C:\\Users\\Pavlo\\Desktop\\assets\\room4.gltf");
  const auto scene = mdl_loader_.GetScene();
  renderer_->SetScene(scene);

  player_.SetModel(&scene->models_rigged[0]);
  player_.SetAnimator(&scene->animator);

  for (int i = 0; i < 3; ++i) {
    // point_lights_.emplace_back(glm::vec3(1.0f), 1.0f, 1.0f);
    // point_lights_.back().SetModelData(mdl_room);
  }

  // auto mdl_fpv =
      // mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Fpv.gltf", 3);

  // player_.SetModelData(scene);
  // player_.SetAnimator(&animator_);

  // directional_light_dir_.push_back(glm::vec3(3.0f, 7.0f, 12.0f)); // TODO: SUN

  CreateCharacterShapes();
  CreateCollisionShapes();

  for (const auto& obj : scene->models) {
    JPH::Ref<JPH::Shape> local_shape;
    JPH::Ref<JPH::Shape> local_shape_door_hinge;

    if (obj.collision_type == Scene::CollisionType::Sphere) {
      float radius = (obj.max[0] - obj.min[0]) * 0.5f;
      JPH::SphereShapeSettings sphere_settings(radius);

      // Safely create via Jolt reference counting
      local_shape = sphere_settings.Create().Get();
      local_shape_door_hinge = sphere_settings.Create().Get(); //TODO: remove (can't)
    }
    else {
      auto half_extend_glm = (obj.max - obj.min) / 2.0f;
      JPH::Vec3 half_extend(half_extend_glm.x, half_extend_glm.y, half_extend_glm.z);
      JPH::Vec3 half_extend_hinge(half_extend_glm.x/5.0f, half_extend_glm.y, half_extend_glm.z/5.0f);
      JPH::BoxShapeSettings box_settings(half_extend);
      local_shape = box_settings.Create().Get();
      JPH::BoxShapeSettings door_hinge_settings(half_extend_hinge);
      local_shape_door_hinge = box_settings.Create().Get();
    }

    auto center_glm = (obj.max + obj.min) / 2.0f;
    JPH::Vec3 local_center(center_glm.x, center_glm.y, center_glm.z);

    if (!local_center.IsNearZero()) {
      JPH::OffsetCenterOfMassShapeSettings offset_settings(local_center, local_shape);
      local_shape = offset_settings.Create().Get();
    }

    JPH::BodyCreationSettings static_body_settings(
      local_shape,
      JPH::RVec3::sZero(),
      JPH::Quat::sIdentity(),
      JPH::EMotionType::Static,
      Layers::NON_MOVING
    );
    JPH::BodyCreationSettings dynamic_body_settings(
      local_shape,
      JPH::RVec3::sZero(),
      JPH::Quat::sIdentity(),
      JPH::EMotionType::Dynamic,
      Layers::MOVING
    );
    JPH::BodyCreationSettings door_hinge_settings(
    local_shape_door_hinge,
          JPH::RVec3::sZero(),
          JPH::Quat::sIdentity(),
          JPH::EMotionType::Static,
          Layers::NON_MOVING
    );

    /// not for jolt (doesn't support dynamic bodies scaling)
    /// auto scale = glm::vec3{data.scale.x, data.scale.y, data.scale.z};
    for (const auto& data : obj.instances) {
      if (obj.type == Scene::Type::Dynamic || obj.type == Scene::Type::Bench) {
        dynamic_body_settings.mPosition = {data.position.x, data.position.y, data.position.z};
        dynamic_body_settings.mRotation = {data.rotation.x, data.rotation.y, data.rotation.z, data.rotation.w};
        dynamic_body_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        dynamic_body_settings.mMassPropertiesOverride.mMass = 10.0f;
        auto collision_id = mBodyInterface->CreateAndAddBody(dynamic_body_settings, JPH::EActivation::Activate);
          static_objects_.push_back({obj, collision_id});
      } else if (obj.type == Scene::Type::Door) {
        dynamic_body_settings.mPosition = {data.position.x, data.position.y, data.position.z};
        dynamic_body_settings.mRotation = {data.rotation.x, data.rotation.y, data.rotation.z, data.rotation.w};
        dynamic_body_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        dynamic_body_settings.mMassPropertiesOverride.mMass = 10.0f;
        auto collision_id = mBodyInterface->CreateAndAddBody(dynamic_body_settings, JPH::EActivation::Activate);
        static_objects_.push_back({obj, collision_id});

        //TODO: hinge body:
        door_hinge_settings.mPosition = {data.position.x, data.position.y, data.position.z - 1.5f};
        door_hinge_settings.mRotation = {data.rotation.x, data.rotation.y, data.rotation.z, data.rotation.w};
        JPH::BodyID b1 = mBodyInterface->CreateAndAddBody(
          door_hinge_settings, JPH::EActivation::DontActivate);

        // Connect the parts with a hinge
        JPH::HingeConstraintSettings hinge;
        hinge.mPoint1 = hinge.mPoint2 = JPH::RVec3(data.position.x, data.position.y, data.position.z - 1.5f);
        hinge.mHingeAxis1 = hinge.mHingeAxis2 = JPH::Vec3::sAxisY();
        mPhysicsSystem->AddConstraint(mBodyInterface->CreateConstraint(&hinge, b1, collision_id));
      } else {
        static_body_settings.mPosition = {data.position.x, data.position.y, data.position.z};
        static_body_settings.mRotation = {data.rotation.x, data.rotation.y, data.rotation.z, data.rotation.w};
        auto collision_id = mBodyInterface->CreateAndAddBody(static_body_settings, JPH::EActivation::DontActivate);
        if (obj.type == Scene::Type::PointLight) {
          point_lights_.push_back({obj, collision_id});
        } else {
          static_objects_.push_back({obj, collision_id});
        }
      }
    }
  }
  int shot_obj_id = mShapeToGeometry.at(JPH::EShapeSubType::Sphere);
  shot_obj_model_ = &scene->models[shot_obj_id];


  // Create 'player' character
  JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
  settings->mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);
  settings->mMaxStrength = 100.0f;
  settings->mShape = mStandingShape;
  settings->mBackFaceMode = JPH::EBackFaceMode::CollideWithBackFaces;
  settings->mCharacterPadding = 0.02f;
  settings->mPenetrationRecoverySpeed = 1.0f;
  settings->mPredictiveContactDistance = 0.1f;
  settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -cCharacterRadiusStanding); // Accept contacts that touch the lower sphere of the capsule
  settings->mEnhancedInternalEdgeRemoval = false;
  settings->mInnerBodyShape = mInnerStandingShape;
  settings->mInnerBodyLayer = Layers::MOVING;
  player_.SetCharacterVirtual(new JPH::CharacterVirtual(settings, JPH::RVec3(0, 5, 0),
    JPH::Quat::sIdentity(), 0, mPhysicsSystem),
    &mCharacterVsCharacterCollision,
    GetCharacterContactListener());
  mCharacterVsCharacterCollision.Add(player_.GetJphCharacter());

  auto npc_pos = JPH::RVec3::sZero();
  for (int i = 0; i < 5; ++i) {
    JPH::Ref<JPH::CharacterVirtual> npc = new JPH::CharacterVirtual(
      settings, npc_pos + JPH::RVec3(i, i + 1, i),
      JPH::Quat::sIdentity(), 0, mPhysicsSystem);
    npc->SetCharacterVsCharacterCollision(&mCharacterVsCharacterCollision);
    mCharacterVsCharacterCollision.Add(npc);
    characters_.emplace_back(scene->models_rigged[0], npc, scene->animator.AddInstance());
  }
}

void CharacterBaseTest::PrePhysicsUpdate(float delta_time) {
  // Update scene time
  auto time = static_cast<float>(glfwGetTime());

  // Stance switch
  if (player_.switch_stance_triggered_) {
    player_.switch_stance_triggered_ = false;
    JPH::CharacterVirtual* player = player_.GetJphCharacter();
    bool is_standing = player->GetShape() == mStandingShape;
    const JPH::Shape *shape = is_standing? mCrouchingShape : mStandingShape;
    if (player->SetShape(shape, 1.5f * mPhysicsSystem->GetPhysicsSettings().mPenetrationSlop, mPhysicsSystem->GetDefaultBroadPhaseLayerFilter(Layers::MOVING), mPhysicsSystem->GetDefaultLayerFilter(Layers::MOVING), { }, { }, *mTempAllocator))
    {
      const JPH::Shape *inner_shape = is_standing? mInnerCrouchingShape : mInnerStandingShape;
      player->SetInnerBodyShape(inner_shape);
    }
  }

  // Animate character virtual
  for (auto& c : characters_) {
    if (c.jph_character_ != nullptr) {
      // Draw the character
      // DrawPaddedCharacter(jph_character_->GetShape(),
      //                     jph_character_->GetCharacterPadding(),
      //                     jph_character_->GetCenterOfMassTransform());

      // Update velocity and apply gravity
      JPH::Vec3 velocity;
      if (c.jph_character_->GetGroundState() ==
          JPH::CharacterVirtual::EGroundState::OnGround)
        velocity = JPH::Vec3::sZero();
      else
        velocity =
            c.jph_character_->GetLinearVelocity() * c.jph_character_->GetUp() +
            mPhysicsSystem->GetGravity() * delta_time;
      velocity += JPH::Sin(time) * cCharacterVelocity;
      c.jph_character_->SetLinearVelocity(velocity);

      // Move character
      JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;
      c.jph_character_->ExtendedUpdate(
          delta_time, mPhysicsSystem->GetGravity(), update_settings,
          mPhysicsSystem->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
          mPhysicsSystem->GetDefaultLayerFilter(Layers::MOVING), {}, {},
          *mTempAllocator);
    }
  }


  // CharacterVirtualTest.h
  // Draw character pre update (the sim is also drawn pre update)
	JPH::RMat44 com = player_.GetJphCharacter()->GetCenterOfMassTransform();
	JPH::RMat44 world_transform = player_.GetJphCharacter()->GetWorldTransform();
#ifdef JPH_DEBUG_RENDERER
	// mCharacter->GetShape()->Draw(mDebugRenderer, com, JPH::Vec3::sOne(), JPH::Color::sGreen, false, true);
#endif // JPH_DEBUG_RENDERER

	// DrawPaddedCharacter(mCharacter->GetShape(), mCharacter->GetCharacterPadding(), com);

	// Remember old position
	JPH::RVec3 old_position = player_.GetJphCharacter()->GetPosition();

	// Settings for our update function

  bool		sEnableWalkStairs = true;
  bool		sEnableStickToFloor = true;

  auto character_up = player_.GetJphCharacter()->GetUp();;

	JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;
	if (!sEnableStickToFloor)
		update_settings.mStickToFloorStepDown = JPH::Vec3::sZero();
	else
		update_settings.mStickToFloorStepDown = -character_up * update_settings.mStickToFloorStepDown.Length();
	if (!sEnableWalkStairs)
		update_settings.mWalkStairsStepUp = JPH::Vec3::sZero();
	else
		update_settings.mWalkStairsStepUp = character_up * update_settings.mWalkStairsStepUp.Length();

	// Update the character position
	player_.GetJphCharacter()->ExtendedUpdate(gDeltaTime,
		-character_up * mPhysicsSystem->GetGravity().Length(),
		update_settings,
		mPhysicsSystem->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
		mPhysicsSystem->GetDefaultLayerFilter(Layers::MOVING),
		{ },
		{ },
		*mTempAllocator);

#ifdef JPH_ENABLE_ASSERTS
	// Validate that our contact list is in sync with that of the character
	// Note that compound shapes could be non convex so we may detect more contacts than have been reported by the character
	// as the character only reports contacts as it is sliding through the world. If 2 sub shapes hit at the same time then
	// most likely only one will be reported as it stops the character and prevents the 2nd one from being seen.
	JPH::uint num_contacts = 0;
	for (const JPH::CharacterVirtual::Contact &c : player_.GetJphCharacter()->GetActiveContacts())
		if (c.mHadCollision)
		{
			JPH_ASSERT(std::find(mActiveContacts.begin(), mActiveContacts.end(), c) != mActiveContacts.end());
			num_contacts++;
		}
  if (num_contacts != mActiveContacts.size())
    std::cout << num_contacts << ' ' << mActiveContacts.size() << std::endl;;
	// JPH_ASSERT(num_contacts == mActiveContacts.size());
#endif
}

void CharacterBaseTest::PostPhysicsUpdate(JPH::Vec3 camera_forward) {
  // Fetch the new ground properties
  // mCharacter->PostSimulation(cCollisionTolerance);
  player_.Update(camera_forward, mPhysicsSystem->GetGravity());
}

JPH::Color DefineColor(JPH::EMotionType body_type, JPH::BodyID body_id) {
  JPH::Color color;
  switch (body_type) {
    case JPH::EMotionType::Static:
      color = JPH::Color::sGrey;
      break;
    case JPH::EMotionType::Kinematic:
      color = JPH::Color::sGreen;
      break;
    case JPH::EMotionType::Dynamic:
      color = JPH::Color::sGetDistinctColor(body_id.GetIndex());
      break;
    default:
      JPH_ASSERT(false);
      color = JPH::Color::sBlack;
      break;
  }
  return color;
}

void AddCharacterDebugInstance(const JPH::BodyLockInterface& bli,
  Renderer* renderer,
  const CharacterBaseTest::ShapeToGeometryMap& shapeToGeometry,
  JPH::BodyID body_id, const Scene::Model* model) {
  JPH::BodyLockRead lock(bli, body_id);
  if (!lock.SucceededAndIsInBroadPhase()) {
    return;
  }
  const JPH::Body& body = lock.GetBody();
  JPH::RMat44 matrix = body.GetCenterOfMassTransform();
  const JPH::Shape* shape = body.GetShape();
  const auto* rt_shape = static_cast<const JPH::RotatedTranslatedShape*>(shape);
  const JPH::Shape* core_shape = rt_shape->GetInnerShape();
  const auto* capsule_shape = static_cast<const JPH::CapsuleShape*>(core_shape);
  float diameter = capsule_shape->GetRadius();
  JPH::Vec3 scale = JPH::Vec3(diameter, diameter, diameter);
  matrix = matrix.PreScaled(scale);
  JPH::AABox bounds = body.GetWorldSpaceBounds();
  const Scene::Model* player_model = model;
  auto start = player_model->primitives_offset;
  auto end = start + player_model->primitives_num;
  int mesh_id = shapeToGeometry.at(core_shape->GetSubType());
  JPH::Color color = DefineColor(body.GetMotionType(), body.GetID());
  for (int i = start; i < end; ++i) {
    renderer->AddInstance(Renderer::InstanceInfo{
    matrix, color, mesh_id, bounds});
  }
}

int GetShapeData(
  const CharacterBaseTest::ShapeToGeometryMap& shapeToGeometry,
  const JPH::Shape* shape, JPH::Vec3& out_scale) {
  while (shape->GetSubType() == JPH::EShapeSubType::OffsetCenterOfMass ||
           shape->GetSubType() == JPH::EShapeSubType::RotatedTranslated) {
    if (shape->GetSubType() == JPH::EShapeSubType::OffsetCenterOfMass) {
      shape = static_cast<const JPH::OffsetCenterOfMassShape*>(shape)->GetInnerShape();
    } else {
      shape = static_cast<const JPH::RotatedTranslatedShape*>(shape)->GetInnerShape();
    }
           }
  out_scale = JPH::Vec3::sReplicate(1.0f);
  if (shape->GetSubType() == JPH::EShapeSubType::Box) {
    out_scale = static_cast<const JPH::BoxShape*>(shape)->GetHalfExtent();
  } else if (shape->GetSubType() == JPH::EShapeSubType::Sphere) {
    out_scale = JPH::Vec3::sReplicate(static_cast<const JPH::SphereShape*>(shape)->GetRadius());
  }
  return shapeToGeometry.at(shape->GetSubType());
}

void CharacterBaseTest::DebugDrawPhysics(bool is_first_face_mode) {
  const JPH::BodyLockInterface& bli = mPhysicsSystem->GetBodyLockInterface();
  auto process_debug_batch = [&](const JPH::BodyID& body_id, auto dispatch_render_fn) {
    JPH::BodyLockRead lock(bli, body_id);
    if (!lock.SucceededAndIsInBroadPhase()) return;
    const JPH::Body& body = lock.GetBody();
    JPH::RMat44 matrix = body.GetCenterOfMassTransform();
    JPH::Vec3 scale;
    int mesh_id = GetShapeData(mShapeToGeometry, body.GetShape(), scale);
    matrix = matrix.PreScaled(scale);
    JPH::AABox bounds = body.GetWorldSpaceBounds();
    JPH::Color color = DefineColor(body.GetMotionType(), body.GetID());
    Renderer::InstanceInfo info{matrix, color, mesh_id, bounds};
    dispatch_render_fn(info);
  };

  for (const auto& s : static_objects_) {
    process_debug_batch(s.body_id_, [&](const auto& info) { renderer_->AddInstance(info); });
  }
  for (const auto& id : shot_objects_) {
    process_debug_batch(id, [&](const auto& info) { renderer_->AddInstance(info); });
  }
  for (const auto& l : point_lights_) {
    process_debug_batch(l.body_id_, [&](const auto& info) { renderer_->AddInstance(&l, info); });
  }
  for (const auto& c : characters_) {
    AddCharacterDebugInstance(bli, renderer_, mShapeToGeometry,
                              c.jph_character_->GetInnerBodyID(), &c.model);
  }
  if (!is_first_face_mode) {
    AddCharacterDebugInstance(bli, renderer_, mShapeToGeometry,
                              player_.GetJphCharacter()->GetInnerBodyID(), player_.GetModel());
  }
}

void AddCharacterSceneInstance(const JPH::BodyLockInterface& bli,
  Renderer* renderer,
  JPH::BodyID body_id, const Scene::Model* model, int bones_offset) {
  JPH::BodyLockRead lock(bli, body_id);
  if (!lock.SucceededAndIsInBroadPhase()) {
    return;
  }
  const JPH::Body& body = lock.GetBody();
  JPH::RMat44 matrix = body.GetCenterOfMassTransform();
  float half_height = (model->max.y - model->min.y) / 2.0f;
  matrix = matrix.PreTranslated(JPH::Vec3(0.0f, -half_height, 0.0f));
  JPH::AABox bounds = body.GetWorldSpaceBounds();
  auto start = model->primitives_offset;
  auto end = start + model->primitives_num;
  for (int i = start; i < end; ++i) {
    renderer->AddCharacter(Renderer::InstanceInfoRigged{
    matrix, JPH::Color::sWhite, i, bounds, bones_offset});
  }
}

void CharacterBaseTest::RenderScene(bool is_first_face_mode) {
  mdl_loader_.GetScene()->animator.Update();

  const JPH::BodyLockInterface& bli = mPhysicsSystem->GetBodyLockInterface();
  auto process_render_batch = [&](const JPH::BodyID& body_id,
                                  const Scene::Model& model,
                                  auto dispatch_render_fn) {
    JPH::BodyLockRead lock(bli, body_id);
    if (!lock.SucceededAndIsInBroadPhase()) return;
    const JPH::Body& body = lock.GetBody();
    JPH::RMat44 matrix = body.GetCenterOfMassTransform();
    JPH::AABox bounds = body.GetWorldSpaceBounds();
    int start = model.primitives_offset;
    int end = start + model.primitives_num;
    for (int i = start; i < end; ++i) {
      int mesh_id = i;
      /// if jph shape, transform wrt its physics
      if (i < mShapeToGeometry.size()) {
        JPH::Vec3 scale;
        mesh_id = GetShapeData(mShapeToGeometry, body.GetShape(), scale);
        matrix = matrix.PreScaled(scale);
      }
      Renderer::InstanceInfo info{matrix, JPH::Color::sWhite, mesh_id, bounds};
      dispatch_render_fn(info);
    }
  };
  for (const auto& s : static_objects_) {
    process_render_batch(s.body_id_, s.model,
      [&](const auto& info) { renderer_->AddInstance(info); });
  }
  for (const auto& id : shot_objects_) {
    process_render_batch(id, *shot_obj_model_,
      [&](const auto& info) { renderer_->AddInstance(info); });
  }
  for (const auto& l : point_lights_) {
    process_render_batch(l.body_id_, l.model,
      [&](const auto& info) { renderer_->AddInstance(&l, info); });
  }
  for (const auto& c : characters_) {
    AddCharacterSceneInstance(bli, renderer_,
      c.jph_character_->GetInnerBodyID(), &c.model,
      mdl_loader_.GetScene()->animator.GetInstance(c.animation_id_).bones_offset);
  }
  if (!is_first_face_mode) {
    AddCharacterSceneInstance(bli, renderer_,
      player_.GetJphCharacter()->GetInnerBodyID(), player_.GetModel(),
      mdl_loader_.GetScene()->animator.GetInstance(
        player_.GetAnimationId()).bones_offset);
  }
}

void CharacterBaseTest::OnContactAdded(const JPH::Body &inBody1,
                                       const JPH::Body &inBody2,
                                       const JPH::ContactManifold &inManifold,
                                       JPH::ContactSettings &ioSettings) {
  // Draw a box around the character when it enters the sensor
  // if (inBody1.GetID() == mSensorBody)
  //   mDebugRenderer->DrawBox(inBody2.GetWorldSpaceBounds(), JPH::Color::sGreen,
  //                           JPH::DebugRenderer::ECastShadow::Off,
  //                           JPH::DebugRenderer::EDrawMode::Wireframe);
  // else if (inBody2.GetID() == mSensorBody)
  //   mDebugRenderer->DrawBox(inBody1.GetWorldSpaceBounds(), JPH::Color::sGreen,
  //                           JPH::DebugRenderer::ECastShadow::Off,
  //                           JPH::DebugRenderer::EDrawMode::Wireframe);
}

void CharacterBaseTest::OnContactPersisted(
    const JPH::Body &inBody1, const JPH::Body &inBody2,
    const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) {
  // Same behavior as contact added
  OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
}

void CharacterBaseTest::DrawPaddedCharacter(const JPH::Shape *inShape,
                                            float inPadding,
                                            JPH::RMat44Arg inCenterOfMass) {
  // if (inShape->GetSubType() == JPH::EShapeSubType::Capsule) {
  //   const JPH::CapsuleShape *capsule =
  //       static_cast<const JPH::CapsuleShape *>(inShape);
  //   mDebugRenderer->DrawCapsule(
  //       inCenterOfMass, capsule->GetHalfHeightOfCylinder(),
  //       capsule->GetRadius() + inPadding, JPH::Color::sGrey,
  //       JPH::DebugRenderer::ECastShadow::Off,
  //       JPH::DebugRenderer::EDrawMode::Wireframe);
  // } else if (inShape->GetSubType() == JPH::EShapeSubType::Cylinder) {
  //   // Not correct as the edges should be rounded
  //   const JPH::CylinderShape *cylinder =
  //       static_cast<const JPH::CylinderShape *>(inShape);
  //   mDebugRenderer->DrawCylinder(
  //       inCenterOfMass, cylinder->GetHalfHeight() + inPadding,
  //       cylinder->GetRadius() + inPadding, JPH::Color::sGrey,
  //       JPH::DebugRenderer::ECastShadow::Off,
  //       JPH::DebugRenderer::EDrawMode::Wireframe);
  // } else if (inShape->GetSubType() == JPH::EShapeSubType::Box) {
  //   // Not correct as the edges should be rounded
  //   const JPH::BoxShape *box = static_cast<const JPH::BoxShape *>(inShape);
  //   JPH::AABox bounds = box->GetLocalBounds();
  //   bounds.ExpandBy(JPH::Vec3::sReplicate(inPadding));
  //   mDebugRenderer->DrawWireBox(inCenterOfMass, bounds, JPH::Color::sGrey);
  // } else if (inShape->GetSubType() == JPH::EShapeSubType::RotatedTranslated) {
  //   const JPH::RotatedTranslatedShape *rt =
  //       static_cast<const JPH::RotatedTranslatedShape *>(inShape);
  //   DrawPaddedCharacter(rt->GetInnerShape(), inPadding, inCenterOfMass);
  // } else if (inShape->GetType() == JPH::EShapeType::Compound) {
  //   const JPH::CompoundShape *compound =
  //       static_cast<const JPH::CompoundShape *>(inShape);
  //   for (const JPH::CompoundShape::SubShape &sub_shape :
  //        compound->GetSubShapes())
  //     DrawPaddedCharacter(sub_shape.mShape, inPadding,
  //                         inCenterOfMass * sub_shape.GetLocalTransformNoScale(
  //                                              JPH::Vec3::sOne()));
  // }
}

glm::vec3 CharacterBaseTest::GetCharacterPosition(
  const JPH::BodyInterface& body_interface) {
  auto id = player_.GetJphCharacter()->GetInnerBodyID();
  JPH::RVec3 position = body_interface.GetPosition(id);
  // JPH::RVec3 com_position = body_interface.GetCenterOfMassPosition(id);
  // JPH::RMat44 transform = body_interface.GetWorldTransform(id);
  return {position.GetX(), position.GetY(), position.GetZ()};
}

void CharacterBaseTest::OnAdjustBodyVelocity(
    const JPH::CharacterVirtual *inCharacter, const JPH::Body &inBody2,
    JPH::Vec3 &ioLinearVelocity, JPH::Vec3 &ioAngularVelocity) {
  // Apply artificial velocity to the character when standing on the conveyor
  // belt
  // if (inBody2.GetID() == mConveyorBeltBody) ioLinearVelocity += JPH::Vec3(0, 0, 2);
}

void CharacterBaseTest::OnContactCommon(const JPH::CharacterVirtual *inCharacter,
                                        const JPH::BodyID &inBodyID2,
                                        const JPH::SubShapeID &inSubShapeID2,
                                        JPH::RVec3Arg inContactPosition,
                                        JPH::Vec3Arg inContactNormal,
                                        JPH::CharacterContactSettings &ioSettings) {
  // Draw a box around the character when it enters the sensor
  // if (inBodyID2 == mSensorBody) {
  //   JPH::AABox box = inCharacter->GetShape()->GetWorldSpaceBounds(
  //       inCharacter->GetCenterOfMassTransform(), JPH::Vec3::sOne());
  //   // mDebugRenderer->DrawBox(box, JPH::Color::sGreen, JPH::DebugRenderer::ECastShadow::Off,
  //   //                         JPH::DebugRenderer::EDrawMode::Wireframe);
  // }

  // Dynamic boxes on the ramp go through all permutations
  // JPH::Array<JPH::BodyID>::const_iterator i =
  //     std::find(mRampBlocks.begin(), mRampBlocks.end(), inBodyID2);
  // if (i != mRampBlocks.end()) {
  //   size_t index = i - mRampBlocks.begin();
  //   ioSettings.mCanPushCharacter = (index & 1) != 0;
  //   ioSettings.mCanReceiveImpulses = (index & 2) != 0;
  // }

  // If we encounter an object that can push the player, enable sliding
  if (inCharacter == player_.GetJphCharacter() && ioSettings.mCanPushCharacter &&
      mPhysicsSystem->GetBodyInterface().GetMotionType(inBodyID2) !=
          JPH::EMotionType::Static)
    player_.AllowSliding(true);
}

void CharacterBaseTest::OnContactAdded(const JPH::CharacterVirtual *inCharacter,
                                       const JPH::BodyID &inBodyID2,
                                       const JPH::SubShapeID &inSubShapeID2,
                                       JPH::RVec3Arg inContactPosition,
                                       JPH::Vec3Arg inContactNormal,
                                       JPH::CharacterContactSettings &ioSettings) {
  OnContactCommon(inCharacter, inBodyID2, inSubShapeID2, inContactPosition,
                  inContactNormal, ioSettings);

  if (inCharacter == player_.GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact added with body %08x, sub shape %08x",
          inBodyID2.GetIndexAndSequenceNumber(), inSubShapeID2.GetValue());
#endif
    JPH::CharacterVirtual::ContactKey c(inBodyID2, inSubShapeID2);
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(), c) !=
        mActiveContacts.end())
      throw std::runtime_error(
          "Got an add contact that should have been a persisted contact");
    mActiveContacts.push_back(c);
  }
}

void CharacterBaseTest::OnContactPersisted(
    const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2,
    const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition,
    JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings &ioSettings) {
  OnContactCommon(inCharacter, inBodyID2, inSubShapeID2, inContactPosition,
                  inContactNormal, ioSettings);

  if (inCharacter == player_.GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact persisted with body %08x, sub shape %08x",
          inBodyID2.GetIndexAndSequenceNumber(), inSubShapeID2.GetValue());
#endif
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(),
                  JPH::CharacterVirtual::ContactKey(inBodyID2, inSubShapeID2)) ==
        mActiveContacts.end())
      throw std::runtime_error(
          "Got a persisted contact that should have been an add contact");
  }
}

void CharacterBaseTest::OnContactRemoved(const JPH::CharacterVirtual *inCharacter,
                                         const JPH::BodyID &inBodyID2,
                                         const JPH::SubShapeID &inSubShapeID2) {
  if (inCharacter == player_.GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact removed with body %08x, sub shape %08x",
          inBodyID2.GetIndexAndSequenceNumber(), inSubShapeID2.GetValue());
#endif
    ContactSet::iterator it =
        std::find(mActiveContacts.begin(), mActiveContacts.end(),
                  JPH::CharacterVirtual::ContactKey(inBodyID2, inSubShapeID2));
    if (it == mActiveContacts.end())
      throw std::runtime_error("Got a remove contact that has not been added");
    mActiveContacts.erase(it);
  }
}

void CharacterBaseTest::OnCharacterContactCommon(
    const JPH::CharacterVirtual *inCharacter,
    const JPH::CharacterVirtual *inOtherCharacter, const JPH::SubShapeID &inSubShapeID2,
    JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
    JPH::CharacterContactSettings &ioSettings) {
  // Characters can only be pushed in their own update
  ioSettings.mCanPushCharacter = inOtherCharacter == player_.GetJphCharacter();
  // If the player can be pushed by the other virtual character, we allow
  // sliding
  if (inCharacter == player_.GetJphCharacter() && ioSettings.mCanPushCharacter)
    player_.AllowSliding(true);
}

void CharacterBaseTest::OnCharacterContactAdded(
    const JPH::CharacterVirtual *inCharacter,
    const JPH::CharacterVirtual *inOtherCharacter, const JPH::SubShapeID &inSubShapeID2,
    JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
    JPH::CharacterContactSettings &ioSettings) {
  OnCharacterContactCommon(inCharacter, inOtherCharacter, inSubShapeID2,
                           inContactPosition, inContactNormal, ioSettings);

  if (inCharacter == player_.GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact added with character %08x, sub shape %08x",
          inOtherCharacter->GetID().GetValue(), inSubShapeID2.GetValue());
#endif
    JPH::CharacterVirtual::ContactKey c(inOtherCharacter->GetID(), inSubShapeID2);
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(), c) !=
        mActiveContacts.end())
      throw std::runtime_error(
          "Got an add contact that should have been a persisted contact");
    mActiveContacts.push_back(c);
  }
}

void CharacterBaseTest::OnCharacterContactPersisted(
    const JPH::CharacterVirtual *inCharacter,
    const JPH::CharacterVirtual *inOtherCharacter, const JPH::SubShapeID &inSubShapeID2,
    JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
    JPH::CharacterContactSettings &ioSettings) {
  OnCharacterContactCommon(inCharacter, inOtherCharacter, inSubShapeID2,
                           inContactPosition, inContactNormal, ioSettings);

  if (inCharacter == player_.GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact persisted with character %08x, sub shape %08x",
          inOtherCharacter->GetID().GetValue(), inSubShapeID2.GetValue());
#endif
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(),
                  JPH::CharacterVirtual::ContactKey(inOtherCharacter->GetID(),
                                               inSubShapeID2)) ==
        mActiveContacts.end())
      throw std::runtime_error(
          "Got a persisted contact that should have been an add contact");
  }
}

void CharacterBaseTest::OnCharacterContactRemoved(
    const JPH::CharacterVirtual *inCharacter, const JPH::CharacterID &inOtherCharacterID,
    const JPH::SubShapeID &inSubShapeID2) {
  if (inCharacter == player_.GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact removed with character %08x, sub shape %08x",
          inOtherCharacterID.GetValue(), inSubShapeID2.GetValue());
#endif
    ContactSet::iterator it = std::find(
        mActiveContacts.begin(), mActiveContacts.end(),
        JPH::CharacterVirtual::ContactKey(inOtherCharacterID, inSubShapeID2));
    if (it == mActiveContacts.end())
      throw std::runtime_error("Got a remove contact that has not been added");
    mActiveContacts.erase(it);
  }
}

void CharacterBaseTest::OnContactSolve(
    const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2,
    const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition,
    JPH::Vec3Arg inContactNormal, JPH::Vec3Arg inContactVelocity,
    const JPH::PhysicsMaterial *inContactMaterial, JPH::Vec3Arg inCharacterVelocity,
    JPH::Vec3 &ioNewCharacterVelocity) {
  // Ignore callbacks for other characters than the player
  if (inCharacter != player_.GetJphCharacter()) return;

  // Don't allow the player to slide down static not-too-steep surfaces when not
  // actively moving and when not on a moving platform
  if (!player_.GetAllowSliding() && inContactVelocity.IsNearZero() &&
      !inCharacter->IsSlopeTooSteep(inContactNormal))
    ioNewCharacterVelocity = JPH::Vec3::sZero();
}
