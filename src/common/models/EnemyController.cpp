#include "EnemyController.h"

EnemyController::EnemyController(
CharacterSharedData* shared_data,
Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::CharacterRig* skin)
: character_(shared_data, scene_node, model, skin) {
  character_.jph_character_->SetCharacterVsCharacterCollision(
  &character_.shared_data_->mCharacterVsCharacterCollision_);
  auto patrol_path = ParseCharacterPath(scene_node);
  SetPatrol(patrol_path);
}

std::vector<JPH::Vec3> EnemyController::ParseCharacterPath(
  Scene::ModelNode* character_node) {
  std::vector<JPH::Vec3> path;
  std::function<void(Scene::ModelNode*)> dfs = [&](Scene::ModelNode* node) {
    if (!node->children.empty()) {
      path.push_back(node->children[0]->local_transform.t);
      dfs(node->children[0]); // only first, keep tricial for now
    }
  };
  dfs(character_node);
  return path;
}

void EnemyController::UpdateLogic(
  const JPH::PhysicsSystem* physics_system, float dt) {
  //UpdatePatrol(physics_system, dt)
}

void EnemyController::UpdatePatrol(
    const JPH::PhysicsSystem* physics_system,
    float delta_time) {
  // if (current_node_index == -1) {
  //   return;
  // }
  // JPH::Vec3 current_pos = jph_character_->GetPosition();
  // JPH::Vec3 intended_velocity = JPH::Vec3::sZero();
  //
  // JPH::Vec3 target_pos = path_nodes[current_node_index];
  // JPH::Vec3 to_target = target_pos - current_pos;
  // to_target.SetY(0.0f);
  // float distance = to_target.Length();
  // if (distance < 0.5f) {
  //   current_node_index = (current_node_index + 1) % path_nodes.size();
  // } else {
  //   JPH::Vec3 move_dir = to_target / distance;
  //   float walk_speed = 2.5f;
  //   intended_velocity = move_dir * walk_speed;
  // }
  //
  // JPH::Vec3 final_velocity;
  // if (jph_character_->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround) {
  //   final_velocity = intended_velocity;
  // } else {
  //   final_velocity =
  //     intended_velocity + (jph_character_->GetLinearVelocity()
  //       .Dot(jph_character_->GetUp()) * jph_character_->GetUp()) +
  //         (physics_system->GetGravity() * delta_time);
  // }
  // final_velocity += external_impulse;
  // float friction = 5.0f; // Higher = stops sliding faster
  // float decay_factor = std::max(0.0f, 1.0f - (friction * delta_time));
  // external_impulse *= decay_factor;
  // if (external_impulse.LengthSq() < 0.01f) {
  //   external_impulse = JPH::Vec3::sZero();
  // }
  // jph_character_->SetLinearVelocity(final_velocity);
}
