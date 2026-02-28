#include "Wirebound.h"

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/NarrowPhaseStats.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include "../io/Window.h"
#include "../render/DebugRendererImp.h"
#include "../ui/UiRenderer.h"
#include "DebugUI.h"

static constexpr unsigned int cNumBodies = 10240;
static constexpr unsigned int cNumBodyMutexes = 0;  // Autodetect
static constexpr unsigned int cMaxBodyPairs = 65536;
static constexpr unsigned int cMaxContactConstraints = 20480;

Wirebound::Wirebound() {
  // Create factory
  JPH::Factory::sInstance = new JPH::Factory;

  // Register physics types with the factory
  JPH::RegisterTypes();

  {
    // Create renderer
    mRenderer = Renderer::sCreate();

    // Init debug renderer
    mDebugRenderer = new DebugRendererImp(mRenderer);

    // Init UI
    mUI = new UiRenderer(mRenderer);
    mUI->SetVisible(false);

    // Init debug UI
    mDebugUI = new DebugUI(mUI, mFont.get());
  }

  // Get initial time
  mLastUpdateTime = std::chrono::high_resolution_clock::now();

  // Limit the render frequency to our simulation frequency so we don't play
  // back the simulation too fast Note that if the simulation frequency > vsync
  // frequency the simulation will slow down as we want to visualize every
  // simulation step. When the simulation frequency is lower than the vsync
  // frequency we will not render a new frame every frame as we want to show the
  // result of the sim and not an interpolated version.
  SetRenderFrequency(mUpdateFrequency);

  // Allocate temp memory
  mTempAllocator = new TempAllocatorImpl(32 * 1024 * 1024);

  // Create job system
  mJobSystem = new JPH::JobSystemThreadPool(
      JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
      std::thread::hardware_concurrency() - 1);
}

Wirebound::~Wirebound() {
  // Clean up
  delete mContactListener;
  delete mPhysicsSystem;
  delete mJobSystem;
  delete mTempAllocator;

  delete mDebugUI;
  delete mUI;
  delete mDebugRenderer;
  delete mRenderer;

  // Unregisters all types with the factory and cleans up the default material
  JPH::UnregisterTypes();

  delete JPH::Factory::sInstance;
  JPH::Factory::sInstance = nullptr;
}

void Wirebound::Start() {
  // Clear anything that is being rendered right now to avoid showing the
  // previous test while initializing the new one
  ClearDebugRenderer();

  // Pop active menus, we might be in the settings menu for the test which will
  // be dangling after restarting the test
  mDebugUI->BackToMain();

  // Store old gravity
  JPH::Vec3 old_gravity = mPhysicsSystem != nullptr
                              ? mPhysicsSystem->GetGravity()
                              : JPH::Vec3(0, -9.81f, 0);

  // Discard old test
  delete mContactListener;
  delete mPhysicsSystem;

  // Create physics system
  mPhysicsSystem = new JPH::PhysicsSystem();
  mPhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs,
                       cMaxContactConstraints, mBroadPhaseLayerInterface,
                       mObjectVsBroadPhaseLayerFilter,
                       mObjectVsObjectLayerFilter);
  mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);

  // Restore gravity
  mPhysicsSystem->SetGravity(old_gravity);

  // Optimize the broadphase to make the first update fast
  mPhysicsSystem->OptimizeBroadPhase();

  // Reset the camera to the original position
  ResetCamera();

  // Start paused
  Pause(true);
  SingleStep();
}

void Wirebound::DrawPhysics() {
#ifdef JPH_DEBUG_RENDERER
  mPhysicsSystem->DrawBodies(mBodyDrawSettings, mDebugRenderer);

  if (mDrawConstraints) mPhysicsSystem->DrawConstraints(mDebugRenderer);

  if (mDrawConstraintLimits)
    mPhysicsSystem->DrawConstraintLimits(mDebugRenderer);

  if (mDrawConstraintReferenceFrame)
    mPhysicsSystem->DrawConstraintReferenceFrame(mDebugRenderer);

  if (mDrawBroadPhaseBounds)
    mDebugRenderer->DrawWireBox(
        mPhysicsSystem->GetBroadPhaseQuery().GetBounds(), JPH::Color::sGreen);
#endif  // JPH_DEBUG_RENDERER

  mTest->DrawBodyLabels();

  // This map collects the shapes that we used this frame
  ShapeToGeometryMap shape_to_geometry;

#ifdef JPH_DEBUG_RENDERER
  if (mDrawGetTriangles)
#endif  // JPH_DEBUG_RENDERER
  {

    // Iterate through all active bodies
    JPH::BodyIDVector bodies;
    mPhysicsSystem->GetBodies(bodies);
    const JPH::BodyLockInterface &bli = mPhysicsSystem->GetBodyLockInterface();
    for (JPH::BodyID b : bodies) {
      // Get the body
      JPH::BodyLockRead lock(bli, b);
      if (lock.SucceededAndIsInBroadPhase()) {
        // Collect all leaf shapes for the body and their transforms
        const JPH::Body &body = lock.GetBody();
        JPH::AllHitCollisionCollector<JPH::TransformedShapeCollector> collector;
        body.GetTransformedShape().CollectTransformedShapes(
            body.GetWorldSpaceBounds(), collector);

        // Draw all leaf shapes
        for (const JPH::TransformedShape &transformed_shape : collector.mHits) {
          JPH::DebugRenderer::GeometryRef geometry;

          // Find geometry from previous frame
          ShapeToGeometryMap::iterator map_iterator =
              mShapeToGeometry.find(transformed_shape.mShape);
          if (map_iterator != mShapeToGeometry.end())
            geometry = map_iterator->second;

          if (geometry == nullptr) {
            // Find geometry from this frame
            map_iterator = shape_to_geometry.find(transformed_shape.mShape);
            if (map_iterator != shape_to_geometry.end())
              geometry = map_iterator->second;
          }

          if (geometry == nullptr) {
            // Geometry not cached
            JPH::Array<JPH::DebugRenderer::Triangle> triangles;

            // Start iterating all triangles of the shape
            JPH::Shape::GetTrianglesContext context;
            transformed_shape.mShape->GetTrianglesStart(
                context, JPH::AABox::sBiggest(), JPH::Vec3::sZero(),
                JPH::Quat::sIdentity(), JPH::Vec3::sOne());
            for (;;) {
              // Get the next batch of vertices
              constexpr int cMaxTriangles = 1000;
              JPH::Float3 vertices[3 * cMaxTriangles];
              int triangle_count = transformed_shape.mShape->GetTrianglesNext(
                  context, cMaxTriangles, vertices);
              if (triangle_count == 0) break;

              // Allocate space for triangles
              size_t output_index = triangles.size();
              triangles.resize(triangles.size() + triangle_count);
              JPH::DebugRenderer::Triangle *triangle = &triangles[output_index];

              // Convert to a renderable triangle
              for (int vertex = 0, vertex_max = 3 * triangle_count;
                   vertex < vertex_max; vertex += 3, ++triangle) {
                // Get the vertices
                JPH::Vec3 v1(vertices[vertex + 0]);
                JPH::Vec3 v2(vertices[vertex + 1]);
                JPH::Vec3 v3(vertices[vertex + 2]);

                // Calculate the normal
                JPH::Float3 normal;
                (v2 - v1)
                    .Cross(v3 - v1)
                    .NormalizedOr(JPH::Vec3::sZero())
                    .StoreFloat3(&normal);

                v1.StoreFloat3(&triangle->mV[0].mPosition);
                triangle->mV[0].mNormal = normal;
                triangle->mV[0].mColor = JPH::Color::sWhite;
                triangle->mV[0].mUV = JPH::Float2(0, 0);

                v2.StoreFloat3(&triangle->mV[1].mPosition);
                triangle->mV[1].mNormal = normal;
                triangle->mV[1].mColor = JPH::Color::sWhite;
                triangle->mV[1].mUV = JPH::Float2(0, 0);

                v3.StoreFloat3(&triangle->mV[2].mPosition);
                triangle->mV[2].mNormal = normal;
                triangle->mV[2].mColor = JPH::Color::sWhite;
                triangle->mV[2].mUV = JPH::Float2(0, 0);
              }
            }

            // Convert to geometry
            geometry = new JPH::DebugRenderer::Geometry(
                mDebugRenderer->CreateTriangleBatch(triangles),
                transformed_shape.mShape->GetLocalBounds());
          }

          // Ensure that we cache the geometry for next frame
          // Don't cache soft bodies as their shape changes every frame
          if (!body.IsSoftBody())
            shape_to_geometry[transformed_shape.mShape] = geometry;

          // Determine color
          JPH::Color color;
          switch (body.GetMotionType()) {
            case JPH::EMotionType::Static:
              color = JPH::Color::sGrey;
              break;

            case JPH::EMotionType::Kinematic:
              color = JPH::Color::sGreen;
              break;

            case JPH::EMotionType::Dynamic:
              color = JPH::Color::sGetDistinctColor(body.GetID().GetIndex());
              break;

            default:
              JPH_ASSERT(false);
              color = JPH::Color::sBlack;
              break;
          }

          // Draw the geometry
          JPH::Vec3 scale = transformed_shape.GetShapeScale();
          bool inside_out = JPH::ScaleHelpers::IsInsideOut(scale);
          JPH::RMat44 matrix =
              transformed_shape.GetCenterOfMassTransform().PreScaled(scale);
          mDebugRenderer->DrawGeometry(
              matrix, color, geometry,
              inside_out ? JPH::DebugRenderer::ECullMode::CullFrontFace
                         : JPH::DebugRenderer::ECullMode::CullBackFace,
              JPH::DebugRenderer::ECastShadow::On,
              body.IsSensor() ? JPH::DebugRenderer::EDrawMode::Wireframe
                              : JPH::DebugRenderer::EDrawMode::Solid);
        }
      }
    }
  }

  // Replace the map with the newly created map so that shapes that we don't
  // draw / were removed are released
  mShapeToGeometry = std::move(shape_to_geometry);
}

void Wirebound::StepPhysics(JPH::JobSystem *inJobSystem) {
  float delta_time = 1.0f / mUpdateFrequency;
  PrePhysicsUpdate(delta_time);
  // Remember start time
  std::chrono::high_resolution_clock::time_point clock_start =
      std::chrono::high_resolution_clock::now();

  // Step the world (with fixed frequency)
  mPhysicsSystem->Update(delta_time, mCollisionSteps, mTempAllocator,
                         inJobSystem);
#ifndef JPH_DISABLE_TEMP_ALLOCATOR
  JPH_ASSERT(static_cast<JPH::TempAllocatorImpl *>(mTempAllocator)->IsEmpty());
#endif  // JPH_DISABLE_TEMP_ALLOCATOR

  // Accumulate time
  std::chrono::high_resolution_clock::time_point clock_end =
      std::chrono::high_resolution_clock::now();
  std::chrono::microseconds duration =
      std::chrono::duration_cast<std::chrono::microseconds>(clock_end -
                                                            clock_start);
  mTotalTime += duration;
  mStepNumber++;
  PostPhysicsUpdate(delta_time);
}

float Wirebound::GetWorldScale() const { return 1.0f; }

// Clear debug lines / triangles / texts that have been accumulated
void Wirebound::ClearDebugRenderer() {
  static_cast<DebugRendererImp *>(mDebugRenderer)->Clear();
  mDebugRendererCleared = true;
}

// Main loop
void Wirebound::Run() { RunLoop(); }

bool Wirebound::RenderFrame() {
  ProccessInput();

  // Calculate delta time
  std::chrono::high_resolution_clock::time_point time =
      std::chrono::high_resolution_clock::now();
  std::chrono::microseconds delta =
      std::chrono::duration_cast<std::chrono::microseconds>(time -
                                                            mLastUpdateTime);
  mLastUpdateTime = time;
  float clock_delta_time = 1.0e-6f * delta.count();
  float world_delta_time = 0.0f;
  if (mRequestedDeltaTime <= 0.0f) {
    // If no fixed frequency update is requested, update with variable time step
    world_delta_time = !mIsPaused || mSingleStep ? clock_delta_time : 0.0f;
    mResidualDeltaTime = 0.0f;
  } else {
    // Else use fixed time steps
    if (mSingleStep) {
      // Single step
      world_delta_time = mRequestedDeltaTime;
    } else if (!mIsPaused) {
      // Calculate how much time has passed since the last render
      world_delta_time = clock_delta_time + mResidualDeltaTime;
      if (world_delta_time < mRequestedDeltaTime) {
        // Too soon, set the residual time and don't update
        mResidualDeltaTime = world_delta_time;
        world_delta_time = 0.0f;
      } else {
        // Update and clamp the residual time to a full update to avoid spiral
        // of death
        mResidualDeltaTime = std::min(mRequestedDeltaTime,
                                      world_delta_time - mRequestedDeltaTime);
        world_delta_time = mRequestedDeltaTime;
      }
    }
  }
  mSingleStep = false;

  // Clear debug lines if we're going to step
  if (world_delta_time > 0.0f) {
    ClearDebugRenderer();
    ProcessInput();

    // Physics world is drawn using debug lines, when not paused
    // Draw state prior to step so that debug lines are created from the same
    // state (the constraints are solved on the current state and then the
    // world is stepped)
    DrawPhysics();

    // Update the physics world
    StepPhysics(mJobSystem);
  }

  // Draw coordinate axis
  if (mDebugRendererCleared)
    mDebugRenderer->DrawCoordinateSystem(JPH::RMat44::sIdentity());

  // For next frame: mark that we haven't cleared debug stuff
  mDebugRendererCleared = false;

  // Update the camera position
  UpdateCamera();

  // Start rendering
  if (!mRenderer->BeginFrame(mWorldCamera, GetWorldScale())) return true;

  // Draw from light
  static_cast<DebugRendererImp *>(mDebugRenderer)->DrawShadowPass();

  // Start drawing normally
  mRenderer->EndShadowPass();

  // Draw debug information
  static_cast<DebugRendererImp *>(mDebugRenderer)->Draw();

  // Draw the frame rate counter
  DrawFPS(clock_delta_time);

  if (mUI->IsVisible()) {
    // Send mouse input to UI
    // bool left_pressed = mMouse->IsLeftPressed();
    // if (left_pressed && !mLeftMousePressed)
    //   mUI->MouseDown(mMouse->GetX(), mMouse->GetY());
    // else if (!left_pressed && mLeftMousePressed)
    //   mUI->MouseUp(mMouse->GetX(), mMouse->GetY());
    // mLeftMousePressed = left_pressed;
    // mUI->MouseMove(mMouse->GetX(), mMouse->GetY());

    HandlePressRelease();

    // Update and draw the menu
    mUI->Update(clock_delta_time);
    mUI->Draw();
  } else {
    // Menu not visible, cancel any mouse operations
    mUI->MouseCancel();
  }

  // Show the frame
  mRenderer->EndFrame();

  return true;
}

void Wirebound::DrawFPS(float inDeltaTime) {
  // Don't divide by zero
  if (inDeltaTime <= 0.0f) return;

  // Update stats
  mTotalDeltaTime += inDeltaTime;
  mNumFrames++;
  if (mNumFrames > 10) {
    mFPS = mNumFrames / mTotalDeltaTime;
    mNumFrames = 0;
    mTotalDeltaTime = 0.0f;
  }

  // Create string
  std::string fps = std::format("{:.1f}", static_cast<double>(mFPS));

  // Get size of text on screen
  auto text_size = mFont->MeasureText(fps);
  int text_w = int(text_size.x * mFont->GetCharHeight());
  int text_h = int(text_size.y * mFont->GetCharHeight());

  // Draw FPS counter
  int x = (gWindowWidth - text_w) / 2 - 20;
  int y = 10;
  mUI->DrawQuad(x - 5, y - 3, text_w + 10, text_h + 6, UiQuad(),
                JPH::Color(0, 0, 0, 128));
  mUI->DrawText(x, y, fps, mFont.get());

  // Draw status string
  if (!mStatusString.empty()) mUI->DrawText(5, 5, mStatusString, mFont.get());

  // Draw paused string if the app is paused
  if (mIsPaused) {
    std::string_view paused_str = "P: Unpause, ESC: Menu";
    auto pause_size_glm = mFont->MeasureText(paused_str);
    auto pause_size = JPH::Float2{pause_size_glm.x, pause_size_glm.y};
    mUI->DrawText(gWindowWidth - 5 - int(pause_size.x * mFont->GetCharHeight()),
                  5, paused_str, mFont.get());
  }
}
