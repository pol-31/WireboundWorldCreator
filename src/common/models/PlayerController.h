#ifndef WIREBOUNDWORLDCREATOR_PLAYERCONTROLLER_H
#define WIREBOUNDWORLDCREATOR_PLAYERCONTROLLER_H

#include "Character.h"
#include "Scene.h"

class PlayerController {
public:
  PlayerController(
JPH::Ref<JPH::CharacterVirtual> jph_character,
CharacterSharedData* shared_data,
const Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::Skin* skin);

  Character* GetBody() { return &character_; }
  const Character* GetBody() const { return &character_; }

  void ProcessMovement(int key, int action);

private:
  void SetMoveForward(bool pressed) { move_forward_ = pressed; }
  void SetMoveBackward(bool pressed) { move_backward_ = pressed; }
  void SetMoveLeft(bool pressed) { move_left_ = pressed; }
  void SetMoveRight(bool pressed) { move_right_ = pressed; }

  JPH::Vec3 GetMovementDirection();

  bool move_forward_ = false;
  bool move_backward_ = false;
  bool move_left_ = false;
  bool move_right_ = false;

  Character character_;
};

#endif  // WIREBOUNDWORLDCREATOR_PLAYERCONTROLLER_H
