#ifndef WIREBOUNDWORLDCREATOR_GAME_H
#define WIREBOUNDWORLDCREATOR_GAME_H

#include <unordered_map>
#include <memory>
#include <chrono>

#include <glm/glm.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystem.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/SubShapeID.h>
#include <Jolt/Skeleton/SkeletonPose.h>

#include "../common/models/Animator.h"
#include "../common/models/ModelLoader.h"
#include "../common/models/PlayerController.h"
#include "../common/models/EnemyController.h"
#include "../common/models/PointLight.h"
#include "../common/models/DirectedLight.h"
#include "../common/models/StaticObject.h"
#include "../common/GlfwContext.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "Layers.h"
#include "Renderer.h"
#include "../common/models/CharacterSharedData.h"
#include "TerrainRenderer.h"
#include "Cubemap.h"
#include "../common/models/Door.h"

#include "UiRenderer.h"

class ContactListenerImpl;
class DebugUI;
class UiScene;

struct RenderSettings {
  bool ssao = true;
  bool shadows = true;
  bool bloom = true;
  bool cull = true;
};

class Game : public JPH::ContactListener,
   public JPH::CharacterContactListener {
 public:
  using ShapeToGeometryMap = std::unordered_map<JPH::EShapeSubType, int>;

  enum class State {
    Game,
    Menu,
    Exit,
  };

  State state_ = State::Game;

  Game();

  ~Game();

  void Run();

 private:
  void Init();

  void DeInit();

  void RunGameLoop();

  void RunMenuLoop();


  int mMaxConcurrentJobs = 1;  // thread::hardware_concurrency();
  JPH::TempAllocator* mTempAllocator = nullptr;
  JPH::JobSystem* mJobSystem = nullptr;
  JPH::JobSystem* mJobSystemValidating = nullptr;
  BPLayerInterfaceImpl mBroadPhaseLayerInterface;
  ObjectVsBroadPhaseLayerFilterImpl mObjectVsBroadPhaseLayerFilter;
  ObjectLayerPairFilterImpl mObjectVsObjectLayerFilter;
  JPH::PhysicsSystem* mPhysicsSystem = nullptr;
  ContactListenerImpl* mContactListener = nullptr;
  JPH::PhysicsSettings mPhysicsSettings;
  JPH::BodyInterface* mBodyInterface = nullptr;

  /// interface & fps

  void RenderInterface();

  void RenderFps();

  void UpdateDeltaTime();

  float frameCount_ = 0;
  float elapsedTime_ = 0;
  float fps_ = 0;

public:
  ModelLoader mdl_loader_;
  Animator animator_;


  TerrainRenderer terrain_renderer_;
  Cubemap cubemap_;
  Camera camera_;

  std::unique_ptr<CharacterSharedData> character_shared_data_; // shapes

  // owns the pointer, modifying the underlying data
  std::unique_ptr<PlayerController> player_;
  std::vector<std::unique_ptr<EnemyController>> characters_;

  // player store ptr to it, so need ptr stability
  std::vector<std::unique_ptr<Weapon>> weapons_;

  // JPH::BodyID (we read bodies only once at Update() of render data matrices)
  std::vector<DirectedLight> dir_lights_;

  GlfwContext global_data_;

  bool render_physics_only_ = false;

  using ContactSet = JPH::Array<JPH::CharacterVirtual::ContactKey>;
  ContactSet mActiveContacts;

  WorldManager world_manager_;
  UiRenderer text_renderer_;
  Renderer renderer_; // in the end
  RenderSettings render_settings_;

  std::vector<Door> doors_;

  std::unique_ptr<DebugUI> mDebugUI;
  std::unique_ptr<UiScene> ui_menu_scene_;

  void CreateBodyForNode(SceneNode* node, SceneZone* zone);

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

void GameScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void GameMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void GameKeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void GameCursorPosCallback(GLFWwindow* window, double xpos, double ypos);

void MenuScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MenuMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void MenuKeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods);

void MenuCursorPosCallback(GLFWwindow* window, double xpos, double ypos);

#endif  // WIREBOUNDWORLDCREATOR_GAME_H
