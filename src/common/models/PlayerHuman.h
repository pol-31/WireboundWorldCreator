#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_PLAYERHUMAN_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_PLAYERHUMAN_H_

#include "../../modes/UiSharedResources.h"
#include "PlayerBase.h"

class PlayerHuman : public PlayerBase {
 public:
  PlayerHuman(UiSharedResources& ui_shared_resources);

  void Update(UiSharedResources& ui_shared_resources) override;

  void ProcessMovement(int key, int action) override;

 private:
  float current_pitch_ = 0.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_PLAYERHUMAN_H_
