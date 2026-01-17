#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_

#include <vector>

#include "../../modes/UiSharedResources.h"
#include "../MapPoint.h"
#include "Animation.h"
#include "AttackEvent.h"
#include "Creature.h"
#include "ModelLoader.h"
#include "Obstacle.h"
#include "PlayerFpv.h"
#include "PlayerHuman.h"
#include "MapMarker.h"

class UiDynamicSprite;

class ModelManager {
 public:
  ModelManager(UiSharedResources& ui_shared_resources);

  void Render();

  void RenderPlacement();

  void RenderOnMap(UiDynamicSprite* sp_player, UiDynamicSprite* sp_enemy,
                   UiDynamicSprite* sp_friend, UiDynamicSprite* sp_neutral,
                   UiDynamicSprite* sp_obstacle);

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

  UiSharedResources& ui_shared_resources_;
  tinygltf::TinyGLTF loader_;
  ModelLoader mdl_loader_;
  HumanAnimator animator_;
  std::vector<AttackEvent> attack_queue_;

  PlayerFpv player_fpv_;
  PlayerHuman player_human_;
  PlayerBase* player_ = nullptr;

  std::vector<Creature> creatures_;

  //// N set of models randomely sparsed on Terrain (non even ID...)... let's...
  Obstacle tree_;
  Obstacle bush_;
  Obstacle tall_grass_;
  Obstacle undergrowth_;

  MapMarker map_point_;

  GLuint player_ubo_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_
