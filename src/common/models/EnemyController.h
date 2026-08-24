#ifndef WIREBOUNDWORLDCREATOR_ENEMYCONTROLLER_H
#define WIREBOUNDWORLDCREATOR_ENEMYCONTROLLER_H

#include "Character.h"
#include "Scene.h"

class EnemyController {
public:
  enum class EnemyType { ThugSplicer, SpiderSplicer, BigDaddy };

  EnemyController(
CharacterSharedData* shared_data,
Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::CharacterRig* skin);

  void UpdateLogic(const JPH::PhysicsSystem* physics_system, float dt);

  Character* GetBody() { return &character_; }
  const Character* GetBody() const { return &character_; }
  bool IsDead() const { return character_.health_ <= 0.0f; }

  void SetPatrol(const std::vector<JPH::Vec3>& patrol) {
    patrol_ = patrol;
  }

private:
  static std::vector<JPH::Vec3> ParseCharacterPath(Scene::ModelNode* character_node);

  void UpdatePatrol(
      const JPH::PhysicsSystem* physics_system,
      float delta_time);

  std::vector<JPH::Vec3> patrol_;
  JPH::Vec3 look_dir_ = JPH::Vec3::sAxisX(); // TODO: same as patrol by default

  EnemyType type_;
  Character character_; // Stored by value INSIDE the controller
};

#endif  // WIREBOUNDWORLDCREATOR_ENEMYCONTROLLER_H
