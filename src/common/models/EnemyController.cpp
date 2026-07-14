#include "EnemyController.h"

EnemyController::EnemyController(
JPH::Ref<JPH::CharacterVirtual> jph_character,
CharacterSharedData* shared_data,
const Scene::ModelNode* scene_node,
const Scene::CharacterData* model,
const Scene::Skin* skin)
: character_(jph_character, shared_data, scene_node, model, skin) {
  //jph_character_->SetCharacterVsCharacterCollision(&shared_data_->mCharacterVsCharacterCollision_);
  //jph_character_->SetListener(shared_data_->contact_listener_);
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
