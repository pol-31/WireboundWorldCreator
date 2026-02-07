#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_

#include <vector>

#include "../../ui/UiRenderData.h"
#include "../MapPoint.h"
#include "Animator.h"
#include "AttackEvent.h"
#include "Fpv.h"
#include "Human.h"
#include "MapMarker.h"
#include "ModelLoader.h"
#include "Obstacle.h"
#include "PlayerFpv.h"
#include "PlayerHuman.h"



#include <Jolt/Jolt.h>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>


namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};  // namespace Layers

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
 public:
  virtual bool ShouldCollide(JPH::ObjectLayer inObject1,
                             JPH::ObjectLayer inObject2) const override {
    switch (inObject1) {
      case Layers::NON_MOVING:
        return inObject2 ==
               Layers::MOVING;  // Non moving only collides with moving
      case Layers::MOVING:
        return true;  // Moving collides with everything
      default:
        JPH_ASSERT(false);
        return false;
    }
  }
};

namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr JPH::uint NUM_LAYERS(2);
};  // namespace BroadPhaseLayers

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
 public:
  BPLayerInterfaceImpl() {
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
  }

  virtual JPH::uint GetNumBroadPhaseLayers() const override {
    return BroadPhaseLayers::NUM_LAYERS;
  }

  virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(
      JPH::ObjectLayer inLayer) const override {
    JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
    return mObjectToBroadPhase[inLayer];
  }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
  virtual const char *GetBroadPhaseLayerName(
      JPH::BroadPhaseLayer inLayer) const override {
    switch ((JPH::BroadPhaseLayer::Type)inLayer) {
      case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
        return "NON_MOVING";
      case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
        return "MOVING";
      default:
        JPH_ASSERT(false);
        return "INVALID";
    }
  }
#endif  // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

 private:
  JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
 public:
  virtual bool ShouldCollide(JPH::ObjectLayer inLayer1,
                             JPH::BroadPhaseLayer inLayer2) const override {
    switch (inLayer1) {
      case Layers::NON_MOVING:
        return inLayer2 == BroadPhaseLayers::MOVING;
      case Layers::MOVING:
        return true;
      default:
        JPH_ASSERT(false);
        return false;
    }
  }
};

class MyContactListener : public JPH::ContactListener {
 public:
  // See: ContactListener
  virtual JPH::ValidateResult OnContactValidate(
      const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset,
      const JPH::CollideShapeResult &inCollisionResult) override {
    // std::cout << "Contact validate callback" << std::endl;
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
  }

  virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2,
                              const JPH::ContactManifold &inManifold,
                              JPH::ContactSettings &ioSettings) override {
    // std::cout << "A contact was added" << std::endl;
  }

  virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                  const JPH::ContactManifold &inManifold,
                                  JPH::ContactSettings &ioSettings) override {
    // std::cout << "A contact was persisted" << std::endl;
  }

  virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override {
    // std::cout << "A contact was removed" << std::endl;
  }
};

class MyBodyActivationListener : public JPH::BodyActivationListener {
 public:
  virtual void OnBodyActivated(const JPH::BodyID &inBodyID,
                               JPH::uint64 inBodyUserData) override {
    // std::cout << "A body got activated" << std::endl;
  }

  virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID,
                                 JPH::uint64 inBodyUserData) override {
    // std::cout << "A body went to sleep" << std::endl;
  }
};

class UiSprite;

class ModelManager {
 public:
  ModelManager(UiRenderData& render_data);

  ~ModelManager() { DeInit(); }

  void Render();

  void RenderPlacement();

  void RenderOnMap(UiSprite* sp_player, UiSprite* sp_enemy, UiSprite* sp_friend,
                   UiSprite* sp_neutral, UiSprite* sp_obstacle);

  void RenderMapPoints(const std::vector<MapPoint>& map_points,
                       glm::vec4 color);

  void RenderPickingMapPoints(const std::vector<MapPoint>& map_points);

  void RenderPicking();

  void RenderMapPoints(const std::vector<MapPoint>& map_points,
                       const std::vector<glm::quat>& rotates,
                       const std::vector<glm::vec3>& scales, glm::vec4 color);

  void RenderPickingMapPoints(const std::vector<MapPoint>& map_points,
                              const std::vector<glm::quat>& rotates,
                              const std::vector<glm::vec3>& scales);

  void Update();

  void UpdateBvh();

  void ProcessEvents();

  const std::vector<std::unique_ptr<ModelData>>& GetLoadedModels() {
    return mdl_loader_.GetLoadedModels();
  }

  void RenderCreaturesAsMapPoints(int creature_id, glm::vec4 color);

  void RenderPickingCreaturesAsMapPoints(int creature_id);

  struct Collider {
    Aabb3D aabb;
    uint32_t owner;
  };

  int BuildBVH(std::vector<BVHNode>& bvh,
               const std::vector<glm::vec3>& aabb_centers,
               std::vector<Collider>& colliders, int start, int end);

  void QueryBVH(const std::vector<BVHNode>& bvh, int node, const Aabb3D& query,
                std::vector<int>& hits);

  Aabb3D ComputeBounds(const std::vector<Collider>& colliders, int start,
                       int end);

 private:
  void Init();

  void DeInit();

  void RenderAabb(RigidBody* entity, const ModelData::Mesh& prim);
  void RenderAabb(MapMarker* entity, const ModelData::Mesh& prim,
    glm::vec2 position = glm::vec2(0.0f),
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
    glm::vec3 scale = glm::vec3(1.0f));


  void RenderAabb(JPH::BodyID id);



  GLuint animation_ubo_ = 0;
  GLuint player_ubo_ = 0;

  std::vector<BVHNode> bvh_;
  std::unordered_map<uint32_t, RigidBody*> entities_;

 public:
  UiRenderData& render_data_;
  tinygltf::TinyGLTF loader_;
  ModelLoader mdl_loader_;
  Animator animator_;
  std::vector<AttackEvent> attack_queue_;

  PlayerHuman player_;

  std::vector<Human> creatures_;
  std::vector<Fpv> fpvs_;

  //// N set of models randomely sparsed on Terrain (non even ID...)... let's...
  Obstacle tree_;
  Obstacle bush_;
  Obstacle tall_grass_;
  Obstacle undergrowth_;

  MapMarker map_point_;

  Shader shader_aabb_;
  Shader shader_aabb_picking_;
  ModelData* mdl_aabb_ = nullptr;

  Shader shader_mdl_;
  Shader shader_animated_mdl_;
  Shader shader_mdl_color_;
  Shader shader_mdl_instanced_;
  Shader shader_mdl_picking_;
  Shader shader_animated_mdl_picking_;
  Shader shader_animated_mdl_color_;

  JPH::TempAllocatorImpl temp_allocator_;
  JPH::JobSystemThreadPool job_system_;
  BPLayerInterfaceImpl broad_phase_layer_interface_;
  ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter_;
  ObjectLayerPairFilterImpl object_vs_object_layer_filter_;
  JPH::PhysicsSystem physics_system_;
  MyBodyActivationListener body_activation_listener_;
  MyContactListener contact_listener_;

  JPH::BodyID player_id_;
  JPH::BodyID enemy_id_;
  JPH::BodyID floor_id_;

  double time_accumulator_ = 0.0;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_
