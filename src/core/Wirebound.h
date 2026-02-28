#ifndef WIREBOUNDWORLDCREATOR_SRC_WIREBOUND_H_
#define WIREBOUNDWORLDCREATOR_SRC_WIREBOUND_H_

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/UnorderedMap.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Skeleton/SkeletonPose.h>

#include "../render/DebugRendererImp.h"
#include "../ui/UiRenderer.h"
#include "DebugUI.h"
#include "Layers.h"

class Wirebound {
 public:
  Wirebound();

  ~Wirebound();

 private:
  // restart physic system
  void Start();

  // Draw the state of the physics system
  void DrawPhysics();

  // Update the physics system with a fixed delta time
  void StepPhysics(JPH::JobSystem *inJobSystem);

  /// Render a frame
  bool RenderFrame();

  /// Draw the frame rate counter
  void DrawFPS(float inDeltaTime);

  /// Clear debug lines / triangles / texts that have been accumulated
  void ClearDebugRenderer();

  /// Enter the main loop
  void Run();

  /// Pause / unpause the simulation
  void Pause(bool inPaused) { mIsPaused = inPaused; }

  /// Programmatically single step the simulation
  void SingleStep() {
    mIsPaused = true;
    mSingleStep = true;
  }

  /// Set the frequency at which we want to render frames
  void SetRenderFrequency(float inFrequency) {
    mRequestedDeltaTime = 1.0f / inFrequency;
  }

  /// Get scale factor for this world, used to boost camera speed and to scale
  /// detail of the shadows
  float GetWorldScale() const;

  /// Debug renderer module
  JPH::DebugRenderer *mDebugRenderer = nullptr;

  /// Render module
  Renderer *mRenderer = nullptr;

  /// Default font
  std::unique_ptr<Font> mFont;

  /// Menu
  UiRenderer *mUI = nullptr;
  DebugUI *mDebugUI = nullptr;

  std::chrono::high_resolution_clock::time_point mLastUpdateTime;
  bool mIsPaused = false;
  bool mSingleStep = false;
  bool mDebugRendererCleared = true;
  bool mLeftMousePressed = false;
  float mFPS = 0.0f;
  float mRequestedDeltaTime = 0.0f;
  float mResidualDeltaTime = 0.0f;
  float mTotalDeltaTime = 0.0f;
  int mNumFrames = 0;

  /// A string that is shown on screen to indicate the status of the application
  std::string mStatusString;

  // Global settings
  float mUpdateFrequency =
      60.0f;  // Physics update frequency, measured in Hz (cycles per second)
  int mCollisionSteps =
      1;  // How many collision detection steps per physics update
  JPH::TempAllocatorImpl *mTempAllocator =
      nullptr;  // Allocator for temporary allocations
  JPH::JobSystem *mJobSystem =
      nullptr;  // The job system that runs physics jobs
  BPLayerInterfaceImpl
      mBroadPhaseLayerInterface;  // The broadphase layer interface that maps
                                  // object layers to broadphase layers
  ObjectVsBroadPhaseLayerFilterImpl
      mObjectVsBroadPhaseLayerFilter;  // Class that filters object vs
                                       // broadphase layers
  ObjectLayerPairFilterImpl
      mObjectVsObjectLayerFilter;  // Class that filters object vs object layers
  JPH::PhysicsSystem *mPhysicsSystem =
      nullptr;  // The physics system that simulates the world
  JPH::ContactListener *mContactListener =
      nullptr;                            // Contact listener implementation
  JPH::PhysicsSettings mPhysicsSettings;  // Main physics simulation settings

  // Drawing settings
#ifdef JPH_DEBUG_RENDERER
  bool mDrawGetTriangles =
      false;  // Draw all shapes using Shape::GetTrianglesStart/Next
  bool mDrawConstraints = false;  // If the constraints should be drawn
  bool mDrawConstraintLimits =
      false;  // If the constraint limits should be drawn
  bool mDrawConstraintReferenceFrame =
      false;  // If the constraint reference frames should be drawn
  bool mDrawBroadPhaseBounds =
      false;  // If the bounds of the broadphase should be drawn
  JPH::BodyManager::DrawSettings
      mBodyDrawSettings;  // Settings for how to draw
                          // bodies from the body manager
  JPH::SkeletonPose::DrawSettings
      mPoseDrawSettings;  // Settings for drawing skeletal poses
#endif                    // JPH_DEBUG_RENDERER

  // Drawing using GetTriangles interface
  using ShapeToGeometryMap = JPH::UnorderedMap<JPH::RefConst<JPH::Shape>,
                                               JPH::DebugRenderer::GeometryRef>;
  ShapeToGeometryMap mShapeToGeometry;

  // Which mode the probe is operating in.
  enum class EProbeMode {
    Pick,
    Ray,
    RayCollector,
    CollidePoint,
    CollideShape,
    CollideShapeWithInternalEdgeRemoval,
    CastShape,
    CollideSoftBody,
    TransformedShape,
    GetTriangles,
    BroadPhaseRay,
    BroadPhaseBox,
    BroadPhaseSphere,
    BroadPhasePoint,
    BroadPhaseOrientedBox,
    BroadPhaseCastBox,
  };

  // Which probe shape to use.
  enum class EProbeShape {
    Sphere,
    Box,
    ConvexHull,
    Capsule,
    TaperedCapsule,
    Cylinder,
    Triangle,
    RotatedTranslated,
    StaticCompound,
    StaticCompound2,
    MutableCompound,
    Mesh,
  };

  // Probe settings
  EProbeMode mProbeMode =
      EProbeMode::Pick;  // Mouse probe mode. Determines what happens under the
                         // crosshair.
  EProbeShape mProbeShape =
      EProbeShape::Sphere;  // Shape to use for the mouse probe.
  JPH::EBackFaceMode mBackFaceModeTriangles =
      JPH::EBackFaceMode::CollideWithBackFaces;  // How to handle back facing
                                                 // triangles when doing a
                                                 // collision probe check.
  JPH::EBackFaceMode mBackFaceModeConvex =
      JPH::EBackFaceMode::CollideWithBackFaces;  // How to handle back facing
                                                 // convex shapes when doing a
                                                 // collision probe check.
  JPH::EActiveEdgeMode mActiveEdgeMode =
      JPH::EActiveEdgeMode::CollideOnlyWithActive;  // How to handle active
                                                    // edges when doing a
                                                    // collision probe check.
  JPH::ECollectFacesMode mCollectFacesMode =
      JPH::ECollectFacesMode::NoFaces;  // If we should collect colliding faces
  float mMaxSeparationDistance =
      0.0f;  // Max separation distance for collide shape test
  bool mTreatConvexAsSolid =
      true;  // For ray casts if the shape should be treated as solid or if the
             // ray should only collide with the surface
  bool mReturnDeepestPoint =
      true;  // For shape casts, when true this will return the deepest point
  bool mUseShrunkenShapeAndConvexRadius =
      false;  // Shrink then expand the shape by the convex radius
  bool mDrawSupportingFace = false;  // Draw the result of GetSupportingFace
  int mMaxHits =
      10;  // The maximum number of hits to request for a collision probe.
  bool mClosestHitPerBody =
      false;  // If we are only interested in the closest hit for every body

  // Timing
  unsigned int mStepNumber = 0;  // Which step number we're accumulating
  std::chrono::microseconds mTotalTime{
      0};  // How many nano seconds we spent simulating
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_WIREBOUND_H_
