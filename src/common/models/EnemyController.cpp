#include "EnemyController.h"

EnemyController::EnemyController(
CharacterSharedData* shared_data,
SceneNode* scene_node,
const Scene::CharacterData* model,
const Scene::CharacterRig* skin)
: character_(shared_data, scene_node, model, skin) {
  character_.jph_character_->SetCharacterVsCharacterCollision(
  &character_.shared_data_->mCharacterVsCharacterCollision_);
  auto patrol_path = ParseCharacterPath(scene_node);
  SetPatrol(patrol_path);
}

void EnemyController::SetPatrol(const std::vector<JPH::Vec3>& patrol) {
  patrol_ = patrol;
  if (patrol.size() > 0) {
    current_node_index_ = 0;
  }
}

// std::vector<JPH::Vec3> EnemyController::ParseCharacterPath(
//   SceneNode* character_node) {
//   std::vector<JPH::Vec3> path;
//   std::function<void(SceneNode*)> dfs = [&](SceneNode* node) {
//     if (!node->children.empty()) {
//       path.push_back(node->children[0]->local_transform.t);
//       //TODO: rotation si the issue
//       dfs(node->children[0]); // only first, keep tricial for now
//     }
//   };
//   dfs(character_node);
//   return path;
// }

std::vector<JPH::Vec3> EnemyController::ParseCharacterPath(SceneNode* character_node) {
  std::vector<JPH::Vec3> path;
  std::function<void(SceneNode*, JPH::Mat44)> dfs = [&](SceneNode* node, JPH::Mat44 parent_matrix) {
    if (!node->children.empty()) {
      SceneNode* child = node->children[0];
      JPH::Mat44 local_matrix = JPH::Mat44::sRotationTranslation(
          child->local_transform.r,
          child->local_transform.t
      );
      JPH::Mat44 global_matrix = parent_matrix * local_matrix;
      path.push_back(global_matrix.GetTranslation());
      dfs(child, global_matrix);
    }
  };
  dfs(character_node, character_node->global_transform);
  return path;
}

void EnemyController::UpdateLogic(
  const JPH::PhysicsSystem* physics_system, float dt) {
  UpdatePatrol(physics_system, dt);
}

void EnemyController::UpdatePatrol(
    const JPH::PhysicsSystem* physics_system,
    float delta_time) {
  if (character_.IsDead()) {
    return;
  }
  if (current_node_index_ == -1) {
    return;
  }
  JPH::Vec3 current_pos = character_.GetPosition();

  JPH::Vec3 target_pos = patrol_[current_node_index_];
  JPH::Vec3 to_target = target_pos - current_pos;
  to_target.SetY(0.0f);
  //to_target.SetZ(0.0f); // stucks on X?
  float distance = to_target.Length();
  character_.movement_direction_ = JPH::Vec3::sZero();
  if (distance < 0.01f) {
    current_node_index_ = (current_node_index_ + 1) % patrol_.size();
  } else {
    character_.movement_direction_ = to_target / distance;
  }
}
