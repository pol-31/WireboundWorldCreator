#include "Game.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/AABoxCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <glad/glad.h>

#include "../common/Callbacks.h"
// #include <Utils/Log.h>
// #include <Utils/ShapeCreator.h>
// #include <Utils/CustomMemoryHook.h>
// #include <Utils/SoftBodyCreator.h>
// #include <Renderer/Renderer.h>

#include "CharacterBaseTest.h"
#include "ContactListenerImpl.h"

JPH_SUPPRESS_WARNINGS_STD_BEGIN
#include <fstream>
JPH_SUPPRESS_WARNINGS_STD_END

JPH_GCC_SUPPRESS_WARNING("-Wswitch")

//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------
static constexpr GLuint cNumBodies = 10240;
static constexpr GLuint cNumBodyMutexes = 0;  // Autodetect
static constexpr GLuint cMaxBodyPairs = 65536;
static constexpr GLuint cMaxContactConstraints = 20480;

namespace JPH {
class JobSystem;
class TempAllocator;
};  // namespace JPH

void Game::UpdateDeltaTime() {
  auto current_frame = static_cast<float>(glfwGetTime());
  gDeltaTime = current_frame - last_frame;
  last_frame = current_frame;

  // Calculate delta time
  // chrono::high_resolution_clock::time_point time =
  // chrono::high_resolution_clock::now(); chrono::microseconds delta =
  // chrono::duration_cast<chrono::microseconds>(time - mLastUpdateTime);
  // mLastUpdateTime = time;
  // float clock_delta_time = 1.0e-6f * delta.count();
  // float world_delta_time = world_delta_time = !mIsPaused || mSingleStep?
  // clock_delta_time : 0.0f; mResidualDeltaTime = 0.0f; mSingleStep = false;
}

void Game::Run() {
  camera_.Reset();

  CheckGlobalData();
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  while (!glfwWindowShouldClose(gWindow)) {
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilMask(0xFF);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    renderer_.Clear();

    UpdateDeltaTime();

    // Reinitialize the job system if the concurrency setting changed
    if (mMaxConcurrentJobs != mJobSystem->GetMaxConcurrency())
      static_cast<JPH::JobSystemThreadPool*>(mJobSystem)
          ->SetNumThreads(mMaxConcurrentJobs - 1);

    JPH::BodyInterface& bi = mPhysicsSystem->GetBodyInterface();
    if (gDeltaTime > 0.0f) {
      // if (mKeyboard->IsKeyPressedAndTriggered(EKey::B, mWasShootKeyPressed))
      // ShootObject();
      mTest->RenderScene();
      // mTest->DebugDrawPhysics();

      // update physics
      float delta_time = 1.0f / mUpdateFrequency;
      mTest->PrePhysicsUpdate(delta_time);
      mPhysicsSystem->Update(delta_time, mCollisionSteps, mTempAllocator,
                             mJobSystem);
      auto fwd = camera_.GetDirectionFront();
      JPH::Vec3 cam_inForward(fwd.x, fwd.y, fwd.z);
      mTest->PostPhysicsUpdate(cam_inForward);
    }
    // mDebugRenderer->DrawCoordinateSystem(RMat44::sIdentity());

    auto player_pos = mTest->GetCharacterPosition(bi);
    // std::cout << player_pos.x << ' ' << player_pos.y << ' ' << player_pos.z << ' ' << std::endl;
    camera_.SetOrigin(player_pos);
    camera_.SetOriginDist(10.0f);
    camera_.MoveRotateViewOriginDist(0.0f);
    camera_.Update();

    renderer_.DrawShadowPass(camera_.GetFrustum());
    renderer_.DrawGeometryPass();

    // mTest->shader_animated_mdl_.Bind();
    // mTest->player_.Render(1.0f);
    // mTest->shader_mdl_.Bind();
    // RenderScene();

    renderer_.DrawLightPass();

    cubemap_.Render();
    // Render();
    // global_data_.UpdateCursorPos();
    // global_data_.UpdateHoveredId();
    // ui_renderer_.Render(&tile_renderer_);
    // picking_fbo_.Bind();
    // ui_renderer_.RenderPicking(&tile_renderer_);
    // RenderPicking();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glfwPollEvents();
    glfwSwapBuffers(gWindow);
  }
}

void Game::RenderScene() {
  mTest->RenderScene();
}

void Game::Init() {
  glfwSetWindowUserPointer(gWindow, reinterpret_cast<void*>(this));

  // global_data_.camera = &camera_;
  // global_data_.tile_renderer = &tile_renderer_;
  // global_data_.picking_fbo = &picking_fbo_;
  // ui_renderer_.SetupGlobalData();

  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glClearColor(0.2f, 0.7f, 0.1f, 1.0f);

  camera_.HideCursor();
  BindCallbacks();
  mLastUpdateTime = std::chrono::high_resolution_clock::now();

  // jph

  // Allocate temp memory
  mTempAllocator = new JPH::TempAllocatorImpl(32 * 1024 * 1024);
  mJobSystem = new JPH::JobSystemThreadPool(
      JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, mMaxConcurrentJobs - 1);
  mJobSystemValidating = new JPH::JobSystemSingleThreaded(JPH::cMaxPhysicsJobs);

  JPH::Vec3 old_gravity = mPhysicsSystem != nullptr
                              ? mPhysicsSystem->GetGravity()
                              : JPH::Vec3(0, -9.81f, 0);

  // Create physics system
  mPhysicsSystem = new JPH::PhysicsSystem();
  mPhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs,
                       cMaxContactConstraints, mBroadPhaseLayerInterface,
                       mObjectVsBroadPhaseLayerFilter,
                       mObjectVsObjectLayerFilter);
  mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);

  // Restore gravity
  mPhysicsSystem->SetGravity(old_gravity);

  // Reset dragging
  mDragAnchor = nullptr;
  mDragBody = JPH::BodyID();
  mDragConstraint = nullptr;
  mDragVertexIndex = ~JPH::uint(0);
  mDragVertexPreviousInvMass = 0.0f;
  mDragFraction = 0.0f;

  // Set new test
  mTest = new CharacterBaseTest(mPhysicsSystem, mJobSystem,
    mTempAllocator, &renderer_);
  mContactListener = new ContactListenerImpl;
  mContactListener->SetNextListener(mTest->GetContactListener());
  mPhysicsSystem->SetContactListener(mContactListener);

  // Optimize the broadphase to make the first update fast
  mPhysicsSystem->OptimizeBroadPhase();

  camera_.Reset();
}

void Game::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetCursorPosCallback(gWindow, CursorPosCallback);
}

void Game::DeInit() { /* ui_renderer_.Serialize(); */ }

void Game::CheckGlobalData() {
  // if (!global_data_.camera || !global_data_.tile_renderer ||
  //     !global_data_.cur_mode || !global_data_.menu ||
  //     !global_data_.picking_fbo || !global_data_.ui_debugger ||
  //     !global_data_.render_data || !global_data_.text_renderer ||
  //     !global_data_.windows) {
  //   throw "init global glfw callback data plz";
  // }
}

void Game::Render() {}

void Game::RenderPicking() {}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
  // bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  // bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    // game.Shoot();
  }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  auto game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, true);
  }
  game->camera_.ProcessMovement(key, action);
  game->mTest->player_.ProcessMovement(key, action);

}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  // std::cout << xpos << ' ' << ypos << std::endl;

  lastX = xpos;
  lastY = ypos;

  auto game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
  game->camera_.MoveRotateViewOrigin(xoffset, yoffset);
}

// RefConst<Shape> SamplesApp::CreateProbeShape()
// {
// 	RefConst<Shape> shape = new SphereShape(0.2f);
// 	JPH_ASSERT(shape != nullptr);
// 	// Scale the shape
// 	Vec3 scale = mScaleShape? shape->MakeScaleValid(mShapeScale) :
// Vec3::sOne(); 	JPH_ASSERT(shape->IsValidScale(scale)); // Double check
// the MakeScaleValid function 	if (!ScaleHelpers::IsNotScaled(scale))
// shape = new ScaledShape(shape, scale);
//
// 	return shape;
// }
//
// RefConst<Shape> SamplesApp::CreateShootObjectShape()
// {
// 	// Get the scale
// 	Vec3 scale = mShootObjectScaleShape? mShootObjectShapeScale :
// Vec3::sOne();
//
// 	// Make it minimally -0.1 or 0.1 depending on the sign
// 	Vec3 clamped_value = Vec3::sSelect(Vec3::sReplicate(-0.1f),
// Vec3::sReplicate(0.1f), Vec3::sGreaterOrEqual(scale, Vec3::sZero()));
// scale = Vec3::sSelect(scale, clamped_value, Vec3::sLess(scale.Abs(),
// Vec3::sReplicate(0.1f)));
//
// 	RefConst<Shape> shape;
//
// 		scale = scale.Swizzle<SWIZZLE_X, SWIZZLE_X, SWIZZLE_X>(); //
// Only uniform scale supported 		shape = new
// SphereShape(GetWorldScale()); 		break;
//
//
//
// 	// Scale shape if needed
// 	if (scale != Vec3::sOne())
// 		shape = new ScaledShape(shape, scale);
//
// 	return shape;
// }
//
// void SamplesApp::ShootObject()
// {
//   // Configure body
//   BodyCreationSettings creation_settings(CreateShootObjectShape(),
//   GetCamera().mPos, Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
//   creation_settings.mMotionQuality = mShootObjectMotionQuality;
//   creation_settings.mFriction = mShootObjectFriction;
//   creation_settings.mRestitution = mShootObjectRestitution;
//   creation_settings.mLinearVelocity = mShootObjectVelocity *
//   GetCamera().mForward;
//
//   // Create body
//   mPhysicsSystem->GetBodyInterface().CreateAndAddBody(creation_settings,
//   EActivation::Activate);
// }
//
// bool SamplesApp::CastProbe(float inProbeLength, float &outFraction, RVec3
// &outPosition, BodyID &outID)
// {
// 	// Determine start and direction of the probe
// 	const CameraState &camera = GetCamera();
// 	RVec3 start = camera.mPos;
// 	Vec3 direction = inProbeLength * camera.mForward;
//
// 	// Define a base offset that is halfway the probe to test getting the
// collision results relative to some offset.
// 	// Note that this is not necessarily the best choice for a base offset,
// but we want something that's not zero
// 	// and not the start of the collision test either to ensure that we'll
// see errors in the algorithm. 	RVec3 base_offset = start + 0.5f *
// direction;
//
// 	// Clear output
// 	outPosition = start + direction;
// 	outFraction = 1.0f;
// 	outID = BodyID();
//
// 	bool had_hit = false;
//   // Create ray
//   RRayCast ray { start, direction };
//
//   // Cast ray
//   RayCastResult hit;
//   had_hit = mPhysicsSystem->GetNarrowPhaseQuery().CastRay(ray, hit,
//   SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::MOVING),
//   SpecifiedObjectLayerFilter(Layers::MOVING));
//
//   // Fill in results
//   outPosition = ray.GetPointOnRay(hit.mFraction);
//   outFraction = hit.mFraction;
//   outID = hit.mBodyID;
//
//   if (had_hit)
//     mDebugRenderer->DrawMarker(outPosition, Color::sYellow, 0.1f);
//   else
//     mDebugRenderer->DrawMarker(camera.mPos + 0.1f * camera.mForward,
//     Color::sRed, 0.001f);
//
//
//
// 	return had_hit;
// }
