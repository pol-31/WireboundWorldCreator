#ifndef WIREBOUNDWORLDCREATOR_ENEMYCONTROLLER_H
#define WIREBOUNDWORLDCREATOR_ENEMYCONTROLLER_H

#include "Character.h"
#include "Scene.h"

class EnemyController {
public:
  enum class EnemyType { ThugSplicer, SpiderSplicer, BigDaddy };

  EnemyController(
CharacterSharedData* shared_data,
SceneNode* scene_node,
const Scene::CharacterData* model,
const Scene::CharacterRig* skin);

  void UpdateLogic(const JPH::PhysicsSystem* physics_system, float dt);

  Character* GetBody() { return &character_; }
  const Character* GetBody() const { return &character_; }

  void SetPatrol(const std::vector<JPH::Vec3>& patrol);

private:
  static std::vector<JPH::Vec3> ParseCharacterPath(SceneNode* character_node);

  void UpdatePatrol(
      const JPH::PhysicsSystem* physics_system,
      float delta_time);

  std::vector<JPH::Vec3> patrol_;
  JPH::Vec3 look_dir_ = JPH::Vec3::sAxisX(); // TODO: same as patrol by default

  int current_node_index_ = -1;
  EnemyType type_;
  Character character_; // Stored by value INSIDE the controller
};

#endif  // WIREBOUNDWORLDCREATOR_ENEMYCONTROLLER_H
