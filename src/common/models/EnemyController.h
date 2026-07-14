#ifndef WIREBOUNDWORLDCREATOR_ENEMYCONTROLLER_H
#define WIREBOUNDWORLDCREATOR_ENEMYCONTROLLER_H

#include "Character.h"
#include "Scene.h"

class EnemyController {
public:
  enum class EnemyType { ThugSplicer, SpiderSplicer, BigDaddy };

  EnemyController(
JPH::Ref<JPH::CharacterVirtual> jph_character,
CharacterSharedData* shared_data,
const Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::Skin* skin);

  void UpdateLogic(const JPH::PhysicsSystem* physics_system, float dt);

  Character* GetBody() { return &character_; }
  const Character* GetBody() const { return &character_; }
  bool IsDead() const { return character_.health_ <= 0.0f; }

  void SetPatrol(const std::vector<JPH::Vec3>& patrol) {
    patrol_ = patrol;
  }

private:
  void UpdatePatrol(
      const JPH::PhysicsSystem* physics_system,
      float delta_time);

  std::vector<JPH::Vec3> patrol_;
  EnemyType type_;
  Character character_; // Stored by value INSIDE the controller
};

#endif  // WIREBOUNDWORLDCREATOR_ENEMYCONTROLLER_H
