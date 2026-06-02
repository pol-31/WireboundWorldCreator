// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include <optional>

#include <Jolt/Jolt.h>
// #include "ContactListenerImpl.h"
#include <Jolt/Core/JobSystem.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "../common/models/ModelLoader.h"
#include "../common/models/Player.h"
#include "../common/models/DirLight.h"
#include "../common/models/Character.h"
#include "../common/models/PointLight.h"
#include "../common/models/StaticObject.h"
#include "Renderer.h"

// Base class for the character tests, initializes the test scene.
class CharacterBaseTest : public JPH::ContactListener,
                          public JPH::CharacterContactListener {
 public:
  CharacterBaseTest(
    JPH::PhysicsSystem *inPhysicsSystem,
    JPH::JobSystem *inJobSystem,
    JPH::TempAllocator *inTempAllocator,
    Renderer *inDebugRenderer);

  ~CharacterBaseTest() {
    DeInit();
  }

  void DebugDrawPhysics(bool is_first_face_mode);

  void RenderScene(bool is_first_face_mode);

  void PrePhysicsUpdate(float delta_time);

  void PostPhysicsUpdate(JPH::Vec3 camera_forward);

  glm::vec3 GetCharacterPosition(const JPH::BodyInterface& body_interface);

  /// scene setup
  JPH::JobSystem *mJobSystem = nullptr;
  JPH::PhysicsSystem *mPhysicsSystem = nullptr;
  JPH::BodyInterface *mBodyInterface = nullptr;
  Renderer *renderer_ = nullptr;
  JPH::TempAllocator *mTempAllocator = nullptr;

 public:

  void CreateCharacterShapes();

  void CreateCollisionShapes();

  void DrawPaddedCharacter(const JPH::Shape *inShape, float inPadding,
                           JPH::RMat44Arg inCenterOfMass);

  void Init();

  void DeInit();

  ModelLoader mdl_loader_;

  Player player_;
  /// Loaded from scene.gltf
  std::vector<PointLight> point_lights_;
  std::vector<DirLight> dir_lights_;
  std::vector<Character> characters_;
  std::vector<StaticObject> static_objects_;


  using ShapeToGeometryMap = std::unordered_map<JPH::EShapeSubType, int>;
  ShapeToGeometryMap mShapeToGeometry;

  JPH::CharacterVsCharacterCollisionSimple mCharacterVsCharacterCollision;

  using ContactSet = JPH::Array<JPH::CharacterVirtual::ContactKey>;
  ContactSet mActiveContacts;

  /// character shapes
  JPH::RefConst<JPH::Shape> mStandingShape;
  JPH::RefConst<JPH::Shape> mCrouchingShape;
  JPH::RefConst<JPH::Shape> mInnerCrouchingShape;
  JPH::RefConst<JPH::Shape> mInnerStandingShape;

  Shader shader_mdl_;
  Shader shader_animated_mdl_;
  // Shader shader_mdl_color_;
  // Shader shader_mdl_instanced_;
  // Shader shader_mdl_picking_;
  // Shader shader_animated_mdl_picking_;
  // Shader shader_animated_mdl_color_;

public:
  /// ContactListener callbacks
  JPH::ContactListener* GetContactListener() {
    return this;
  }
  JPH::CharacterContactListener* GetCharacterContactListener() {
    return this;
  }
  void OnContactAdded(const JPH::Body &inBody1,
                              const JPH::Body &inBody2,
                              const JPH::ContactManifold &inManifold,
                              JPH::ContactSettings &ioSettings) override;
  void OnContactPersisted(const JPH::Body &inBody1,
                                  const JPH::Body &inBody2,
                                  const JPH::ContactManifold &inManifold,
                                  JPH::ContactSettings &ioSettings) override;

  /// CharacterContactListener callbacks
  void OnAdjustBodyVelocity(const JPH::CharacterVirtual *inCharacter,
                                    const JPH::Body &inBody2, JPH::Vec3 &ioLinearVelocity,
                                    JPH::Vec3 &ioAngularVelocity) override;
  void OnContactAdded(const JPH::CharacterVirtual *inCharacter,
                              const JPH::BodyID &inBodyID2,
                              const JPH::SubShapeID &inSubShapeID2,
                              JPH::RVec3Arg inContactPosition,
                              JPH::Vec3Arg inContactNormal,
                              JPH::CharacterContactSettings &ioSettings) override;
  void OnContactPersisted(
      const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2,
      const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition,
      JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings &ioSettings) override;
  void OnContactRemoved(const JPH::CharacterVirtual *inCharacter,
                                const JPH::BodyID &inBodyID2,
                                const JPH::SubShapeID &inSubShapeID2) override;
  void OnCharacterContactAdded(
      const JPH::CharacterVirtual *inCharacter,
      const JPH::CharacterVirtual *inOtherCharacter, const JPH::SubShapeID &inSubShapeID2,
      JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
      JPH::CharacterContactSettings &ioSettings) override;
  void OnCharacterContactPersisted(
      const JPH::CharacterVirtual *inCharacter,
      const JPH::CharacterVirtual *inOtherCharacter, const JPH::SubShapeID &inSubShapeID2,
      JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
      JPH::CharacterContactSettings &ioSettings) override;
  void OnCharacterContactRemoved(
      const JPH::CharacterVirtual *inCharacter,
      const JPH::CharacterID &inOtherCharacterID,
      const JPH::SubShapeID &inSubShapeID2) override;
  void OnContactSolve(
      const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2,
      const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition,
      JPH::Vec3Arg inContactNormal, JPH::Vec3Arg inContactVelocity,
      const JPH::PhysicsMaterial *inContactMaterial, JPH::Vec3Arg inCharacterVelocity,
      JPH::Vec3 &ioNewCharacterVelocity) override;

 protected:
  void OnContactCommon(const JPH::CharacterVirtual *inCharacter,
                       const JPH::BodyID &inBodyID2, const JPH::SubShapeID &inSubShapeID2,
                       JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
                       JPH::CharacterContactSettings &ioSettings);
  void OnCharacterContactCommon(const JPH::CharacterVirtual *inCharacter,
                                const JPH::CharacterVirtual *inOtherCharacter,
                                const JPH::SubShapeID &inSubShapeID2,
                                JPH::RVec3Arg inContactPosition,
                                JPH::Vec3Arg inContactNormal,
                                JPH::CharacterContactSettings &ioSettings);
};
