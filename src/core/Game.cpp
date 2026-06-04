#include "Game.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <Jolt/Jolt.h>
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
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>

#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>

#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/RayCast.h>

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

JPH::RefConst<JPH::Shape> Game::CreateShootObjectShape() {
  JPH::Vec3 scale = mShootObjectScaleShape ? mShootObjectShapeScale : JPH::Vec3::sOne();

  JPH::Vec3 clamped_value =
      JPH::Vec3::sSelect(JPH::Vec3::sReplicate(-0.1f), JPH::Vec3::sReplicate(0.1f),
                    JPH::Vec3::sGreaterOrEqual(scale, JPH::Vec3::sZero()));
  scale = JPH::Vec3::sSelect(scale, clamped_value,
                        JPH::Vec3::sLess(scale.Abs(), JPH::Vec3::sReplicate(0.1f)));

  JPH::RefConst<JPH::Shape> shape;

  scale = scale.Swizzle<JPH::SWIZZLE_X, JPH::SWIZZLE_X,
                            JPH::SWIZZLE_X>();  // Only uniform scale supported
  shape = new JPH::SphereShape(0.1f);

  if (scale != JPH::Vec3::sOne()) shape = new JPH::ScaledShape(shape, scale);

  return shape;
}

void Game::ShootObject() {
  auto glm_pos = camera_.GetPosition();
  auto pos = JPH::Vec3(glm_pos.x, glm_pos.y, glm_pos.z);
  auto glm_forward = camera_.GetDirectionFront();
  auto forward = JPH::Vec3(glm_forward.x, glm_forward.y, glm_forward.z);
  float offset_distance = 0.6f;
  pos += forward * offset_distance;
  JPH::BodyCreationSettings creation_settings(
      CreateShootObjectShape(), pos, JPH::Quat::sIdentity(),
      JPH::EMotionType::Dynamic, Layers::MOVING);
  creation_settings.mMotionQuality = mShootObjectMotionQuality;
  creation_settings.mFriction = mShootObjectFriction;
  creation_settings.mRestitution = mShootObjectRestitution;
  creation_settings.mLinearVelocity = mShootObjectVelocity * forward;
  auto body_id = mPhysicsSystem->GetBodyInterface().CreateAndAddBody(
    creation_settings, JPH::EActivation::Activate);
  mTest->AddShotShere(body_id);
}

class IgnoreSingleBodyFilter : public JPH::BodyFilter {
public:
  IgnoreSingleBodyFilter(const JPH::BodyID& inIgnoreMe) : mIgnoreMe(inIgnoreMe) {}
  virtual bool ShouldCollide(const JPH::BodyID& inBodyID) const override {
    return inBodyID != mIgnoreMe; // Skip the player
  }
private:
  JPH::BodyID mIgnoreMe;
};

bool Game::CastProbe(float inProbeLength, float &outFraction,
                           JPH::RVec3 &outPosition, JPH::BodyID &outID) {
  auto glm_pos = camera_.GetPosition();
  auto pos = JPH::Vec3(glm_pos.x, glm_pos.y, glm_pos.z);
  auto glm_forward = camera_.GetDirectionFront();
  auto forward = JPH::Vec3(glm_forward.x, glm_forward.y, glm_forward.z);

  JPH::RVec3 start = pos;
  JPH::Vec3 direction = inProbeLength * forward;

  // Clear output
  outPosition = start + direction;
  outFraction = 1.0f;
  outID = JPH::BodyID();

  bool had_hit = false;

  IgnoreSingleBodyFilter player_filter(mTest->player_.GetJphCharacter()->GetInnerBodyID());

  JPH::RRayCast ray{start, direction};
  JPH::RayCastResult hit;
  had_hit = mPhysicsSystem->GetNarrowPhaseQuery().CastRay(
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
    std::cout << "cast probe id : " << hit.mBodyID.GetIndex() << ' ' << std::boolalpha << had_hit << std::noboolalpha << std::endl;
  }
  return had_hit;
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
      static_cast<JPH::JobSystemThreadPool *>(mJobSystem)
          ->SetNumThreads(mMaxConcurrentJobs - 1);

    JPH::BodyInterface &bi = mPhysicsSystem->GetBodyInterface();


    auto glm_pos = camera_.GetPosition();
    auto camera_pos = JPH::Vec3(glm_pos.x, glm_pos.y, glm_pos.z);
    auto glm_forward = camera_.GetDirectionFront();
    auto camera_forward = JPH::Vec3(glm_forward.x, glm_forward.y, glm_forward.z);
    const float cDragRayLength = 40.0f;


    if (gDeltaTime > 0.0f) {
      if (shoot_object_triggered_) {
        shoot_object_triggered_ = false;
        ShootObject();
      }

      /// --- --- --- --- ---
      // Allow the user to drag rigid/soft bodies around
      if (mDragConstraint == nullptr && mDragVertexIndex == ~JPH::uint(0)) {
        // Not dragging yet
        JPH::RVec3 hit_position;
        if (glfwGetKey(gWindow, GLFW_KEY_F) == GLFW_PRESS && CastProbe(cDragRayLength, mDragFraction, hit_position, mDragBody)) {
          // Target body must be dynamic
            JPH::BodyLockWrite lock(mPhysicsSystem->GetBodyLockInterface(),
                               mDragBody);
            if (lock.Succeeded()) {
              JPH::Body &drag_body = lock.GetBody();
              if (drag_body.IsSoftBody()) {
                JPH::SoftBodyMotionProperties *mp =
                    static_cast<JPH::SoftBodyMotionProperties *>(
                        drag_body.GetMotionProperties());

                // Find closest vertex
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

                // Make the vertex kinematic
                JPH::SoftBodyVertex &v = mp->GetVertex(mDragVertexIndex);
                mDragVertexPreviousInvMass = v.mInvMass;
                v.mInvMass = 0.0f;
              } else if (drag_body.IsDynamic()) {
                // Create constraint to drag body
                JPH::DistanceConstraintSettings settings;
                settings.mPoint1 = settings.mPoint2 = hit_position;
                settings.mLimitsSpringSettings.mFrequency = 2.0f; // div by world_scale==1
                settings.mLimitsSpringSettings.mDamping = 1.0f;

                // Construct fixed body for the mouse constraint
                // Note that we don't add it to the world since we don't want
                // anything to collide with it, we just need an anchor for a
                // constraint
                JPH::Body *drag_anchor = bi.CreateBody(JPH::BodyCreationSettings(
                    new JPH::SphereShape(0.01f), hit_position, JPH::Quat::sIdentity(),
                    JPH::EMotionType::Static, Layers::NON_MOVING));
                mDragAnchor = drag_anchor;

                // Construct constraint that connects the drag anchor with the
                // body that we want to drag
                mDragConstraint = settings.Create(*drag_anchor, drag_body);
                mPhysicsSystem->AddConstraint(mDragConstraint);
              }
            }
        }
      } else {
        if (glfwGetKey(gWindow, GLFW_KEY_F) != GLFW_PRESS) {
          // If key released, destroy constraint
          if (mDragConstraint != nullptr) {
            mPhysicsSystem->RemoveConstraint(mDragConstraint);
            mDragConstraint = nullptr;
          }

          // Destroy drag anchor
          if (mDragAnchor != nullptr) {
            bi.DestroyBody(mDragAnchor->GetID());
            mDragAnchor = nullptr;
          }

          // Release dragged vertex
          if (mDragVertexIndex != ~JPH::uint(0)) {
            // Restore vertex mass
            JPH::BodyLockWrite lock(mPhysicsSystem->GetBodyLockInterface(),
                               mDragBody);
            if (lock.Succeeded()) {
              JPH::Body &body = lock.GetBody();
              JPH_ASSERT(body.IsSoftBody());
              JPH::SoftBodyMotionProperties *mp =
                  static_cast<JPH::SoftBodyMotionProperties *>(
                      body.GetMotionProperties());
              mp->GetVertex(mDragVertexIndex).mInvMass =
                  mDragVertexPreviousInvMass;
            }
            mDragVertexIndex = ~JPH::uint(0);
            mDragVertexPreviousInvMass = 0;
          }

          // Forget the drag body
          mDragBody = JPH::BodyID();
        } else {
          // Else drag the body to the new position
          JPH::RVec3 new_pos = camera_pos +
                          cDragRayLength * mDragFraction * camera_forward;

          switch (bi.GetBodyType(mDragBody)) {
            case JPH::EBodyType::RigidBody:
              bi.SetPositionAndRotation(mDragAnchor->GetID(), new_pos,
                                        JPH::Quat::sIdentity(),
                                        JPH::EActivation::DontActivate);
              break;

            case JPH::EBodyType::SoftBody: {
              JPH::BodyLockWrite lock(mPhysicsSystem->GetBodyLockInterface(),
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
                              gDeltaTime;
              }
            } break;
          }

          // Activate other body
          bi.ActivateBody(mDragBody);
        }
      }
      /// --- --- --- --- ---

      bool first_face_mode = camera_.IsFirstFaceMode();
      if (render_only_physics_) {
        mTest->DebugDrawPhysics(first_face_mode);
      } else {
        mTest->RenderScene(first_face_mode);
      }

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
    auto head_pos = player_pos;
    float head_height =
        (mTest->player_.GetModel()->max.y - mTest->player_.GetModel()->min.y) *
        0.9f;
    head_pos.y += head_height;
    // std::cout << player_pos.x << ' ' << player_pos.y << ' ' << player_pos.z
    // << ' ' << std::endl;
    camera_.Update(head_pos);
    // TODO: update player jph rotation

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

void Game::Init() {
  glfwSetWindowUserPointer(gWindow, reinterpret_cast<void *>(this));

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
  mTest = new CharacterBaseTest(mPhysicsSystem, mJobSystem, mTempAllocator,
                                &renderer_);
  mContactListener = new ContactListenerImpl;
  mContactListener->SetNextListener(mTest->GetContactListener());
  mPhysicsSystem->SetContactListener(mContactListener);

  // Optimize the broadphase to make the first update fast
  mPhysicsSystem->OptimizeBroadPhase();

  camera_.Reset();
  camera_.SetOriginDist(10.0f);
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

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  auto game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
  game->camera_.ZoomOriginDist(yoffset);
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  auto game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
  // bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  // bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    // game.Shoot();
  }
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  auto game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, true);
    } else if (key == GLFW_KEY_F1) {
      game->camera_.SwitchFaceMode();
    } else if (key == GLFW_KEY_F2) {
      game->SwitchRenderMode();
    } else if (key == GLFW_KEY_E) {
      game->shoot_object_triggered_ = true;
    }
  }

  // game->camera_.ProcessMovement(key, action);
  game->mTest->player_.ProcessMovement(key, action);
}

void CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  // std::cout << xpos << ' ' << ypos << std::endl;

  lastX = xpos;
  lastY = ypos;

  auto game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));

  game->camera_.MoveRotateView(xoffset, yoffset);
  // game->camera_.MoveRotateViewOrigin(xoffset, yoffset);
}
