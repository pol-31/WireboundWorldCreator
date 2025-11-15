#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_

#include <vector>

#include "../../modes/UiSharedResources.h"
#include "ModelLoader.h"
#include "Creature.h"
#include "Obstacle.h"
#include "Player.h"

class ModelManager {
 public:
  ModelManager(UiSharedResources& ui_shared_resources);

  void Render();

  void RenderPicking();

  void Update();

  /// Creature management

  void Spawn(const ModelData& model_data, size_t num = 1);

  void Kill(int id);

  // public is easier, but we could make 1000s of foo(){creature->foo()}
  std::vector<Creature> creatures_;
  /// What is obstacle?
  //// N set of models randomely sparsed on Terrain (non even ID...)... let's...
  /// ---> PLACEMENT! Let's do placement!
  std::vector<Obstacle> obstacles_;
  Player player_;

 private:
  UiSharedResources& ui_shared_resources_;
  ModelLoader mdl_loader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELMANAGER_H_
