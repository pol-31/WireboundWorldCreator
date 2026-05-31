#ifndef WIREBOUNDWORLDCREATOR_GAME_H
#define WIREBOUNDWORLDCREATOR_GAME_H

#include <unordered_map>
#include <chrono>

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/SubShapeID.h>
#include <Jolt/Skeleton/SkeletonPose.h>

#include "../common/GlfwContext.h"
#include "../io/Camera.h"
#include "../io/Window.h"
// #include "CharacterBaseTest.h"
// #include "ContactListenerImpl.h"
#include "Cubemap.h"
#include "Layers.h"
#include "Renderer.h"

class ContactListenerImpl;
class CharacterBaseTest;

class Game {
 public:
  Game() { Init(); }

  ~Game() { DeInit(); }

  void Run();

  /// input
  Camera camera_;  // "mouse"
  // keyboard == callbacks
  // window global (gWindow)

  Cubemap cubemap_;
  GlfwContext global_data_;

  /// remder
  Renderer renderer_;

  std::chrono::high_resolution_clock::time_point mLastUpdateTime;
  float mFPS = 0.0f;
  float mRequestedDeltaTime = 0.0f;
  float mResidualDeltaTime = 0.0f;
  float mTotalDeltaTime = 0.0f;
  int mNumFrames = 0;

  CharacterBaseTest* mTest = nullptr;


 private:
  void Init();

  void DeInit();

  void BindCallbacks();

  /// throw an exception in case of uninitialized global_data_ members
  void CheckGlobalData();

  void Render();

  void RenderPicking();

  void UpdateDeltaTime();

  void DebugDrawPhysics();

  void UpdateInstances();

  void RenderScene();

  // Global settings
  int mMaxConcurrentJobs = 1;  // thread::hardware_concurrency();
  float mUpdateFrequency = 60.0f;
  int mCollisionSteps = 1;
  JPH::TempAllocator* mTempAllocator = nullptr;
  JPH::JobSystem* mJobSystem = nullptr;
  JPH::JobSystem* mJobSystemValidating = nullptr;
  BPLayerInterfaceImpl mBroadPhaseLayerInterface;
  ObjectVsBroadPhaseLayerFilterImpl mObjectVsBroadPhaseLayerFilter;
  ObjectLayerPairFilterImpl mObjectVsObjectLayerFilter;
  JPH::PhysicsSystem* mPhysicsSystem = nullptr;
  ContactListenerImpl* mContactListener = nullptr;
  JPH::PhysicsSettings mPhysicsSettings;

  bool mScaleShape = false;  // If the shape is scaled or not. When true
                             // mShapeScale is taken into account.
  JPH::Vec3 mShapeScale =
      JPH::Vec3::sOne();  // Scale in local space for the probe shape.
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

  float mShootObjectVelocity = 20.0f;  // Speed at which objects are ejected
  JPH::EMotionQuality mShootObjectMotionQuality =
      JPH::EMotionQuality::Discrete;  // Motion quality for the object that
                                      // we're shooting
  float mShootObjectFriction = 0.2f;  // Friction for the object that is shot
  float mShootObjectRestitution =
      0.0f;  // Restitution for the object that is shot
  bool mShootObjectScaleShape = false;  // If the shape should be scaled
  JPH::Vec3 mShootObjectShapeScale =
      JPH::Vec3::sOne();  // Scale of the object to shoot
  bool mWasShootKeyPressed =
      false;  // Remembers if the shoot key was pressed last frame

  // Mouse dragging
  JPH::Body* mDragAnchor =
      nullptr;  // Rigid bodies only: A anchor point for the distance
                // constraint. Corresponds to the current crosshair position.
  JPH::BodyID mDragBody;  // The body ID of the body that the user is currently
                          // dragging.
  JPH::Ref<JPH::Constraint>
      mDragConstraint;  // Rigid bodies only: The distance constraint that
                        // connects the body to be dragged and the anchor point.
  JPH::uint mDragVertexIndex =
      ~JPH::uint(0);  // Soft bodies only: The vertex index of the body that the
                      // user is currently dragging.
  float mDragVertexPreviousInvMass =
      0.0f;  // Soft bodies only: The inverse mass of the vertex that the user
             // is currently dragging.
  float mDragFraction;  // Fraction along cDragRayLength (see cpp) where the hit
                        // occurred. This will be combined with the crosshair
                        // position to get a 3d anchor point.

  // Timing
  JPH::uint mStepNumber = 0;  // Which step number we're accumulating
  std::chrono::microseconds mTotalTime{0};
};

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

#endif  // WIREBOUNDWORLDCREATOR_GAME_H
