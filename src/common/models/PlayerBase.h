#ifndef WIREBOUNDWORLDCREATOR_PLAYERBASE_H
#define WIREBOUNDWORLDCREATOR_PLAYERBASE_H

#include <glm/glm.hpp>

#include "../../modes/UiSharedResources.h"
#include "RigidBody.h"

class PlayerBase : public RigidBody {
 public:
  PlayerBase(UiSharedResources& ui_shared_resources)
      : ui_shared_resources_(ui_shared_resources) {}

  virtual ~PlayerBase() = default;

  virtual void ProcessMovement(int key, int action) = 0;

  void SetMoveForward(bool pressed) { move_forward_ = pressed; }
  void SetMoveBackward(bool pressed) { move_backward_ = pressed; }
  void SetMoveLeft(bool pressed) { move_left_ = pressed; }
  void SetMoveRight(bool pressed) { move_right_ = pressed; }

 protected:
  UiSharedResources& ui_shared_resources_;

  bool move_forward_ = false;
  bool move_backward_ = false;
  bool move_left_ = false;
  bool move_right_ = false;

  glm::vec3 velocity_ = glm::vec3(0.0f);
  glm::vec3 desired_direction_ = glm::vec3(0.0f);
  float desired_magnitude_ = 0.0f;
  float acceleration_ = 5.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_PLAYERBASE_H
