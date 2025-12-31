#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_

#include <vector>

#include "../../modes/UiSharedResources.h"
#include "ModelLoader.h"
#include "Creature.h"
#include "Obstacle.h"
#include "Player.h"

class UiDynamicSprite;

class ModelManager {
 public:
  ModelManager(UiSharedResources& ui_shared_resources);

  void Render();

  void RenderOnMap(
      UiDynamicSprite* sp_player,
      UiDynamicSprite* sp_enemy,
      UiDynamicSprite* sp_friend,
      UiDynamicSprite* sp_neutral,
      UiDynamicSprite* sp_obstacle);

  void RenderPicking();

  void Update();

  /// Creature management

  void Spawn(GLuint vao_id, glm::vec2 position);

  void Kill(int id);

  const std::vector<std::unique_ptr<ModelData>>& GetLoadedModels() {
    return mdl_loader_.GetLoadedModels();
  }

  std::vector<Creature> creatures_;
  //// N set of models randomely sparsed on Terrain (non even ID...)... let's...
  Obstacle tree_;
  Obstacle bush_;
  Obstacle tall_grass_;
  Obstacle undergrowth_;

  Obstacle map_point_;

  Player player_;

 private:
  UiSharedResources& ui_shared_resources_;
  ModelLoader mdl_loader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_
