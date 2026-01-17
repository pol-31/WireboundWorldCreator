#ifndef WIREBOUNDWORLDCREATOR_PLAYERFPV_H
#define WIREBOUNDWORLDCREATOR_PLAYERFPV_H

#include "../../modes/UiSharedResources.h"
#include "PlayerBase.h"

class PlayerFpv : public PlayerBase {
 public:
  PlayerFpv(UiSharedResources& ui_shared_resources);

  void Update(UiSharedResources& ui_shared_resources) override;

  void ProcessMovement(int key, int action) override;

  void SetRotateCcw(bool pressed) { rotate_ccw_ = pressed; }
  void SetRotateCw(bool pressed) { rotate_cw_ = pressed; }

 private:
  float yaw_ = 0.0f;
  float pitch_ = 0.0f;
  float roll_ = 0.0f;

  float current_pitch_ = 0.0f;
  bool rotate_ccw_ = false;
  bool rotate_cw_ = false;
};

#endif  // WIREBOUNDWORLDCREATOR_PLAYERFPV_H
