#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_PLAYER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_PLAYER_H_

#include <glm/glm.hpp>

#include "../../modes/UiSharedResources.h"
#include "RigidBody.h"

class Camera;
struct ModelData;

class Player : public RigidBody {
 public:
  Player(UiSharedResources& ui_shared_resources);

  void Render(UiSharedResources& ui_shared_resources);

  void Update(UiSharedResources& ui_shared_resources);

  [[nodiscard]] const ModelData* GetModelData() const noexcept {
    return model_data_;
  }

  void SetModelData(const ModelData* model) {
    model_data_ = model;
  }

  /**
   * why not SetFront/Side 1/-1?
   * because we can dynamically change the move direction, what
   * requires full Reset()! But if 1/-1 and we've changed the camera
   * yaw it leads to +1 at one direction and -1 at another.
   * and it just keep moving until you trying to find that 8.0452 degrees
   * But anyway with this approach we have an issue:
   * left_press right_press right release -> left is gone
   * */
  void SetMoveForward(float magnitude = 1.0f);

  void SetMoveBackward(float magnitude = 1.0f);

  void SetMoveLeft(float magnitude = 1.0f);

  void SetMoveRight(float magnitude = 1.0f);

  void ResetMoveForward();

  void ResetMoveBackward();

  void ResetMoveLeft();

  void ResetMoveRight();

  void UpdateMods(bool mod_ctrl, bool mod_shift);

  static const float gSpeedSlow;
  static const float gSpeedModerate;
  static const float gSpeedFast;

 private:
  void UpdateCamera(UiSharedResources& ui_shared_resources);

  const ModelData* model_data_ = nullptr;
  glm::vec3 front_move_to_ = glm::vec3(0.0f);
  glm::vec3 side_move_to_ = glm::vec3(0.0f);
  bool move_forward_ = false;
  bool move_left_ = false;
  Camera*& camera_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_PLAYER_H_
