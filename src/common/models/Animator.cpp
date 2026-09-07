#include "Animator.h"

#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../../io/Window.h"
#include "Character.h"

const int Animator::gMaxBones = 1000;

// int FindFrame(const std::vector<float>& times, float t) {
//   if (t <= times[0]) return 0;
//
//   auto count = times.size();
//   for (int i = 0; i < count - 1; ++i) {
//     if (t < times[i + 1]) return i;
//   }
//
//   return count - 2;  // last valid segment
// }

int FindFrame(const std::vector<float>& times, float t) {
  // 1. Guard against empty or single-frame animations
  if (times.size() <= 1) return 0;

  // 2. Early exit if we are before or exactly at the start
  if (t <= times[0]) return 0;

  // 3. Cast to int to prevent unsigned underflow math
  int count = static_cast<int>(times.size());

  for (int i = 0; i < count - 1; ++i) {
    if (t < times[i + 1]) return i;
  }

  // 4. Return the last valid starting frame for interpolation
  return count - 2;
}

void WrapTime(Animator::CoreInstance& instance, const std::vector<float>& times) {
  auto count = times.size();
  // Safeguard: If there's no animation data or only 1 frame, clamp and return.
  if (count <= 1) {
    if (count == 1) instance.time = times[0];
    return;
  }

  float start = times[0];
  float end = times[count - 1];
  float duration = end - start;

  // Safeguard: Prevent division by zero in fmod if duration is extremely small
  if (duration <= 0.00001f) {
    instance.time = start;
    return;
  }

  if (instance.time > end) {
    if (instance.is_looped) {
      // Preserve the overflow time using fmod
      instance.time = start + std::fmod(instance.time - start, duration);
    } else {
      // Clamp to the end and mark as idle
      instance.is_idle = true;
      instance.time = end;
    }
  }
  // Optional: Handle the case where time is less than start
  // (useful if your game allows playing animations in reverse)
  else if (instance.time < start) {
    if (instance.is_looped) {
      instance.time = end - std::fmod(start - instance.time, duration);
    } else {
      instance.time = start;
    }
  }
}

void Animator::Init() {
  glDeleteBuffers(1, &ssbo_);
  glGenBuffers(1, &ssbo_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * gMaxBones,
               nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, ssbo_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  Clear();
}

void Animator::DeInit() {
  glDeleteBuffers(1, &ssbo_);
}

void Animator::Clear() {
  std::vector<glm::mat4> joints_zero = std::vector(gMaxBones, glm::mat4(1.0f));

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
  // glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, joints_zero.size() * sizeof(glm::mat4),
  // joints_zero.data());
  glBufferData(GL_SHADER_STORAGE_BUFFER,
             joints_zero.size() * sizeof(glm::mat4),
             joints_zero.data(),
             GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

size_t Animator::AddInstanceCharacter(
      const SceneNode* root_node,
      const std::vector<SceneNode*>* nodes,
      const Scene::CharacterRig* skin,
      const Character* character) {
  for (int i = 0; i < instances_characters_.size(); ++i) {
    auto& instance = instances_characters_[i];
    if (!instance.core_instance.is_alive) {
      instance = CharacterInstance(); // resets is_dead as well
      instance.core_instance.root_node = root_node;
      instance.core_instance.nodes = nodes;
      instance.skin = skin;
      instance.character = character;
      instance.core_instance.is_alive = true;
      instance.has_pistol = true;
      return i;
    }
  }
  CharacterInstance instance;
  instance.core_instance.root_node = root_node;
  instance.core_instance.nodes = nodes;
  instance.skin = skin;
  instance.character = character;
  instance.core_instance.current_locals.resize(nodes->size());
  instance.core_instance.current_locals_default.resize(nodes->size());
  instance.core_instance.current_globals.resize(nodes->size());
  instance.core_instance.current_joint_matrices.resize(skin->core_rig.joints.size());
  instance.has_pistol = true;
  instances_characters_.push_back(instance);
  return instances_characters_.size() - 1;
}

size_t Animator::AddInstanceWeapon(
      const SceneNode* root_node,
      const std::vector<SceneNode*>* nodes,
      const Scene::WeaponRig* skin) {
  for (int i = 0; i < instances_weapons_.size(); ++i) {
    auto& instance = instances_weapons_[i];
    if (!instance.core_instance.is_alive) {
      instance = WeaponInstance(); // resets is_dead as well
      instance.core_instance.root_node = root_node;
      instance.core_instance.nodes = nodes;
      instance.skin = skin;
      instance.core_instance.is_alive = true;
      return i;
    }
  }
  WeaponInstance instance;
  instance.core_instance.root_node = root_node;
  instance.core_instance.nodes = nodes;
  instance.skin = skin;
  instance.core_instance.current_locals.resize(nodes->size());
  instance.core_instance.current_locals_default.resize(nodes->size());
  instance.core_instance.current_globals.resize(nodes->size());
  instance.core_instance.current_joint_matrices.resize(skin->core_rig.joints.size());
  instances_weapons_.push_back(instance);
  return instances_weapons_.size() - 1;
}

void Animator::RemoveInstanceCharacter(int id) {
  instances_characters_[id].core_instance.is_alive = false;
}

void Animator::RemoveInstanceWeapon(int id) {
  instances_weapons_[id].core_instance.is_alive = false;
}

void Animator::StartCharacter(int instance_id, CharacterAnimType type, bool looped) {
  auto& data = instances_characters_[instance_id];
  auto new_type = data.skin->mapping.at(type);
  if (new_type == data.core_instance.type && !data.core_instance.is_idle) {
    return;
  }
  data.state = CharacterState::FullBodyAction;
  data.core_instance.time = 0.0f;
  data.core_instance.is_looped = looped; // TODO: separate
  data.core_instance.type = new_type;
  data.core_instance.is_idle = false;
}

void Animator::StartWeapon(int instance_id, WeaponAnimType type, bool looped) {
  auto& data = instances_weapons_[instance_id];
  auto new_type = data.skin->mapping.at(type);
  if (new_type == data.core_instance.type && !data.core_instance.is_idle) {
    return;
  }
  data.core_instance.time = 0.0f;
  data.core_instance.is_looped = looped;
  data.core_instance.type = new_type;
  data.core_instance.is_idle = false;
}

struct CharacterBlendingPoses {
  CharacterAnimType idle;
  CharacterAnimType idle_no_arm;
  CharacterAnimType forward;
  CharacterAnimType backward;
  CharacterAnimType left;
  CharacterAnimType right;
  CharacterAnimType jump;
};

static CharacterBlendingPoses cPoseWalk = {
  CharacterAnimType::Idle,
  CharacterAnimType::IdleNoArm,
  CharacterAnimType::WalkForward,
  CharacterAnimType::WalkBackward,
  CharacterAnimType::WalkLeft,
  CharacterAnimType::WalkRight,
  CharacterAnimType::Jump,
};
static CharacterBlendingPoses cPoseRun = {
  CharacterAnimType::Idle,
  CharacterAnimType::IdleNoArm,
  CharacterAnimType::RunForward,
  CharacterAnimType::RunBackward,
  CharacterAnimType::RunLeft,
  CharacterAnimType::RunRight,
  CharacterAnimType::MoveJump,
};
static CharacterBlendingPoses cPoseCrouch = {
  CharacterAnimType::IdleCrouch,
  CharacterAnimType::IdleCrouch,
  CharacterAnimType::CrouchForward,
  CharacterAnimType::CrouchBackward,
  CharacterAnimType::CrouchLeft,
  CharacterAnimType::CrouchRight,
  CharacterAnimType::Jump,
};

CharacterBlendingPoses GetCharacterBlendingPose(JPH::Vec3 velocity, bool aiming) {
  auto hor_velocity = JPH::Vec3(velocity.GetX(), 0.0f, velocity.GetZ());
  auto hor_velocity_mag = hor_velocity.Length();
  CharacterAnimType type;
  if (hor_velocity_mag < Character::cWalkSpeed + 1.0f) {
      return cPoseWalk;
  } else {
      return cPoseRun;
  }
}

void Animator::CompensateLowerToUpperSpines(CharacterInstance& instance) {
  /// --- DISTRIBUTED 3-AXIS UPPER BODY ALIGNMENT ---
  auto hips_idx   = instance.skin->hips_id;
  auto spine0_idx = instance.skin->spine0_id;
  auto spine1_idx = instance.skin->spine1_id;
  auto spine2_idx = instance.skin->spine2_id;

  // 1. Fetch current raw animated local rotations
  JPH::Quat q_hips   = instance.core_instance.current_locals[hips_idx].r;
  JPH::Quat q_spine0 = instance.core_instance.current_locals[spine0_idx].r;
  JPH::Quat q_spine1 = instance.core_instance.current_locals[spine1_idx].r;
  JPH::Quat q_spine2 = instance.core_instance.current_locals[spine2_idx].r;

  // 2. Compute RAW animated model-space transforms
  JPH::Quat raw_m_hips   = q_hips;
  JPH::Quat raw_m_spine0 = raw_m_hips   * q_spine0;
  JPH::Quat raw_m_spine1 = raw_m_spine0 * q_spine1;
  JPH::Quat raw_m_spine2 = raw_m_spine1 * q_spine2;

  // 3. Fetch default rest pose locals
  JPH::Quat d_spine0 = instance.core_instance.current_locals_default[spine0_idx].r;
  JPH::Quat d_spine1 = instance.core_instance.current_locals_default[spine1_idx].r;
  JPH::Quat d_spine2 = instance.core_instance.current_locals_default[spine2_idx].r;

  // 4. Compute FULL IDEAL model-space chain
  JPH::Quat ideal_hips   = JPH::Quat::sRotation(JPH::Vec3::sAxisX(), JPH::DegreesToRadians(-90.0f));
  JPH::Quat ideal_spine0 = ideal_hips   * d_spine0;
  JPH::Quat ideal_spine1 = ideal_spine0 * d_spine1;
  JPH::Quat ideal_spine2 = ideal_spine1 * d_spine2;

  // 5. Calculate sequential blend weights (1/3, 2/3, 3/3)
  float w0 = (1.0f / 3.0f) * instance.armed_weight;
  float w1 = (2.0f / 3.0f) * instance.armed_weight;
  float w2 = (3.0f / 3.0f) * instance.armed_weight;

  // 6. Compute TARGET model-space for each bone by blending RAW towards IDEAL
  JPH::Quat target_m_spine0 = raw_m_spine0.SLERP(ideal_spine0, w0);
  JPH::Quat target_m_spine1 = raw_m_spine1.SLERP(ideal_spine1, w1);
  JPH::Quat target_m_spine2 = raw_m_spine2.SLERP(ideal_spine2, w2);

  // 7. Extract new local rotations sequentially based on the newly corrected parents
  // Spine0 connects to the untouched Hips
  instance.core_instance.current_locals[spine0_idx].r = (raw_m_hips.Inversed() * target_m_spine0).Normalized();
  // Spine1 connects to the new target Spine0
  instance.core_instance.current_locals[spine1_idx].r = (target_m_spine0.Inversed() * target_m_spine1).Normalized();
  // Spine2 connects to the new target Spine1
  instance.core_instance.current_locals[spine2_idx].r = (target_m_spine1.Inversed() * target_m_spine2).Normalized();

  // 8. Neutralize local translation dips evenly across ALL spine joints
  for (auto idx : {spine0_idx, spine1_idx, spine2_idx}) {
      const auto& a = instance.core_instance.current_locals[idx].t;
      const auto& b = instance.core_instance.current_locals_default[idx].t;
      instance.core_instance.current_locals[idx].t = a + (b - a) * instance.armed_weight;
  }
}

void Animator::Update(bool skip) {
  std::vector<JPH::Mat44> all_jointMatrices;
  // Assuming ~65 bones per instance TODO: differs for the gun and for the character
  all_jointMatrices.reserve(
    (instances_characters_.size() + instances_weapons_.size()) * 80);
  for (auto& instance : instances_characters_) {
    if (!instance.core_instance.is_alive) {
      return;
    }
    if (!instance.core_instance.is_idle) {
      instance.core_instance.time += gDeltaTime * speed_;
    }
    instance.core_instance.bones_offset = all_jointMatrices.size();
    auto prev_input_dir = instance.prev_input_dir;
    auto input_dir = instance.character->GetMoveDirectionRaw();
    float character_speed = input_dir.Length();

    if (instance.state == CharacterState::Normal) {
      InitLocalsCharacter(instance, instance.skin->core_rig);
      CompensateLowerToUpperSpines(instance);
      ApplyDeltas(instance, prev_input_dir);
    } else {
      // fullbodymotion
      InitLocalsWeapon(instance.core_instance, instance.skin->core_rig);
      if (instance.core_instance.is_idle && !instance.character->IsDead()) {
        instance.state = CharacterState::Normal;
      }
    }

    ComputeGlobals(instance.core_instance);
    BuildJointMatrices(instance.core_instance, instance.skin->core_rig);
    all_jointMatrices.insert(all_jointMatrices.end(),
      instance.core_instance.current_joint_matrices.begin(),
      instance.core_instance.current_joint_matrices.end());
  }
  for (auto& instance : instances_weapons_) {
    if (!instance.core_instance.is_alive) {
      return;
    }
    if (!instance.core_instance.is_idle) {
      instance.core_instance.time += gDeltaTime * speed_;
    }
    instance.core_instance.bones_offset = all_jointMatrices.size();
    InitLocalsWeapon(instance.core_instance, instance.skin->core_rig);
    ComputeGlobals(instance.core_instance);
    BuildJointMatrices(instance.core_instance, instance.skin->core_rig);
    all_jointMatrices.insert(all_jointMatrices.end(),
      instance.core_instance.current_joint_matrices.begin(),
      instance.core_instance.current_joint_matrices.end());
  }
  if (all_jointMatrices.size() > gMaxBones) {
    std::cerr << "too much bones for ssbo" << std::endl;
    all_jointMatrices.resize(gMaxBones);
  }
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER,
           0,
           all_jointMatrices.size() * sizeof(glm::mat4),
           all_jointMatrices.data());
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

static JPH::Vec3 ReadVec3(const std::vector<float>& v, int index) {
  return JPH::Vec3(
      v[index * 3 + 0],
      v[index * 3 + 1],
      v[index * 3 + 2]);
}

static JPH::Quat ReadQuat(const std::vector<float>& v, int index) {
  return JPH::Quat(
      v[index * 4 + 0],
      v[index * 4 + 1],
      v[index * 4 + 2],
      v[index * 4 + 3]);
}

void Animator::ApplyDeltas(CharacterInstance& instance, JPH::Vec3 prev_move_dir) {
  // 1. Update the Armed Weight
  float raise_speed = 6.0f;
  // Ensure we dynamically target 1.0f or 0.0f based on character state
  bool is_aiming = instance.character->IsAiming();
  float target_armed_weight = is_aiming ? 1.0f : 0.0f;

  if (instance.armed_weight < target_armed_weight) {
    instance.armed_weight = std::min(instance.armed_weight + raise_speed * gDeltaTime, target_armed_weight);
  } else if (instance.armed_weight > target_armed_weight) {
    instance.armed_weight = std::max(instance.armed_weight - raise_speed * gDeltaTime, target_armed_weight);
  }

  // 2. Spine / Body Alignment Logic (Unchanged)
  float aim_yaw = instance.character->GetHeadYaw();
  float aim_pitch = instance.character->GetHeadPitch();
  if (instance.character->GetMoveDirection() != JPH::Vec3::sZero()) {
    auto strafe = instance.character->GetMoveDirectionRaw().GetZ();
    float turn_speed = 10.0f;
    float move_diff = std::remainder(aim_yaw + 90.0f - instance.body_yaw_, 360.0f);
    float diagonal_factor =
      instance.character->GetMoveDirectionRaw().GetX() > 0.0f ? 1.0f : -1.0f;

    if (instance.strafing_) {
      if (std::abs(strafe) != 1.0f) {
        instance.strafing_ = false;
        if (strafe > 0.0f) {
          move_diff =  std::remainder(move_diff - diagonal_factor * 45.0f, 360.0f);
        } else if (strafe < 0.0f) {
          move_diff = std::remainder(move_diff + diagonal_factor * 45.0f, 360.0f);
        }
      }
    } else {
      if (std::abs(strafe) == 1.0f) {
        if (prev_move_dir == JPH::Vec3::sZero()) {
          instance.strafing_ = true;
        } else {
          if (strafe > 0.0f) {
            move_diff =  std::remainder(move_diff + diagonal_factor * 90.0f, 360.0f);
          } else if (strafe < 0.0f) {
            move_diff = std::remainder(move_diff - diagonal_factor * 90.0f, 360.0f);
          }
        }
      } else {
        if (strafe > 0.0f) {
          move_diff =  std::remainder(move_diff - diagonal_factor * 45.0f, 360.0f);
        } else if (strafe < 0.0f) {
          move_diff = std::remainder(move_diff + diagonal_factor * 45.0f, 360.0f);
        }
      }
    }
    instance.body_yaw_ += move_diff * (turn_speed * gDeltaTime);
    instance.body_yaw_ = std::remainder(instance.body_yaw_, 360.0f);
  }

  float diff = std::remainder(aim_yaw + 90.0f - instance.body_yaw_, 360.0f);
  if (diff > 90.0f) {
    instance.body_yaw_ += (diff - 90.0f);
    diff = 90.0f;
    instance.body_yaw_ = std::remainder(instance.body_yaw_, 360.0f);
  } else if (diff < -90.0f) {
    instance.body_yaw_ += (diff + 90.0f);
    diff = -90.0f;
    instance.body_yaw_ = std::remainder(instance.body_yaw_, 360.0f);
  }

  float yaw_weight = -diff / 90.0f;
  float pitch_weight = std::clamp(aim_pitch / 90.0f, -1.0f, 1.0f);

  // 3. Blend and Apply Deltas
  const auto& def_deltas = instance.skin->default_deltas;
  const auto& pis_deltas = instance.skin->pistol_deltas;
  float w = instance.armed_weight;

  for (size_t i = 0; i < instance.core_instance.current_locals.size(); ++i) {
    // --- UNARMED CALCULATION ---
    JPH::Quat yaw_def = JPH::Quat::sIdentity();
    if (yaw_weight > 0.0f) yaw_def = JPH::Quat::sIdentity().SLERP(def_deltas.right[i].r, yaw_weight);
    else if (yaw_weight < 0.0f) yaw_def = JPH::Quat::sIdentity().SLERP(def_deltas.left[i].r, -yaw_weight);

    JPH::Quat pitch_def = JPH::Quat::sIdentity();
    if (pitch_weight > 0.0f) pitch_def = JPH::Quat::sIdentity().SLERP(def_deltas.up[i].r, pitch_weight);
    else if (pitch_weight < 0.0f) pitch_def = JPH::Quat::sIdentity().SLERP(def_deltas.down[i].r, -pitch_weight);

    JPH::Quat comb_def = ((yaw_def * pitch_def).Normalized() * def_deltas.center[i].r).Normalized();

    JPH::Vec3 t_def = def_deltas.center[i].t;
    if (yaw_weight > 0.0f) t_def += def_deltas.right[i].t * yaw_weight;
    else if (yaw_weight < 0.0f) t_def += def_deltas.left[i].t * -yaw_weight;
    if (pitch_weight > 0.0f) t_def += def_deltas.up[i].t * pitch_weight;
    else if (pitch_weight < 0.0f) t_def += def_deltas.down[i].t * -pitch_weight;

    // --- ARMED CALCULATION ---
    JPH::Quat yaw_pis = JPH::Quat::sIdentity();
    if (yaw_weight > 0.0f) yaw_pis = JPH::Quat::sIdentity().SLERP(pis_deltas.right[i].r, yaw_weight);
    else if (yaw_weight < 0.0f) yaw_pis = JPH::Quat::sIdentity().SLERP(pis_deltas.left[i].r, -yaw_weight);

    JPH::Quat pitch_pis = JPH::Quat::sIdentity();
    if (pitch_weight > 0.0f) pitch_pis = JPH::Quat::sIdentity().SLERP(pis_deltas.up[i].r, pitch_weight);
    else if (pitch_weight < 0.0f) pitch_pis = JPH::Quat::sIdentity().SLERP(pis_deltas.down[i].r, -pitch_weight);

    JPH::Quat comb_pis = ((yaw_pis * pitch_pis).Normalized() * pis_deltas.center[i].r).Normalized();

    JPH::Vec3 t_pis = pis_deltas.center[i].t;
    if (yaw_weight > 0.0f) t_pis += pis_deltas.right[i].t * yaw_weight;
    else if (yaw_weight < 0.0f) t_pis += pis_deltas.left[i].t * -yaw_weight;
    if (pitch_weight > 0.0f) t_pis += pis_deltas.up[i].t * pitch_weight;
    else if (pitch_weight < 0.0f) t_pis += pis_deltas.down[i].t * -pitch_weight;

    // --- FINAL BLEND ---
    // Smoothly transition between the unarmed delta state and armed delta state
    JPH::Quat final_comb_delta = comb_def.SLERP(comb_pis, w).Normalized();
    JPH::Vec3 final_t_delta = (t_def * (1.0f - w)) + (t_pis * w);

    // Apply to current locals
    auto new_rot = final_comb_delta * instance.core_instance.current_locals[i].r;
    if (new_rot.LengthSq() > 0.0001f) {
      instance.core_instance.current_locals[i].r = new_rot.Normalized();
    } else {
      instance.core_instance.current_locals[i].r = JPH::Quat::sIdentity();
    }

    instance.core_instance.current_locals[i].t += final_t_delta;
  }
}

void Animator::EvaluateWeightedAnimation(
    CharacterInstance& instance,
    const Scene::Animation& animation,
    float weight) {
  if (weight <= 0.001f) return;

  // 1. Create a temporary pose array filled with default transforms for THIS animation
  std::vector<SceneNodePose> temp_pose = instance.core_instance.current_locals_default;

  float current_time = instance.core_instance.time;
  WrapTime(instance.core_instance, animation.samplers[animation.channels[0].sampler].times);

  // 2. OVERWRITE the defaults with animated channels (Do NOT use += here)
  for (const auto& channel : animation.channels) {
    const auto& sampler = animation.samplers[channel.sampler];
    const auto& times = sampler.times;
    const auto& values = sampler.values;
    int node = channel.target_node;

    int frame = FindFrame(times, current_time);
    int next = std::min(frame + 1, int(times.size() - 1));
    float alpha = 0.0f;
    if (times[next] > times[frame]) {
      alpha = (current_time - times[frame]) / (times[next] - times[frame]);
    }

    if (channel.target_path == "translation") {
      auto a = ReadVec3(values, frame);
      auto b = ReadVec3(values, next);
      temp_pose[node].t = a + alpha * (b - a); // Assignment, not addition!
    } else if (channel.target_path == "rotation") {
      auto a = ReadQuat(values, frame);
      auto b = ReadQuat(values, next);
      temp_pose[node].r = a.SLERP(b, alpha).Normalized();
    } else if (channel.target_path == "scale") {
      auto a = ReadVec3(values, frame);
      auto b = ReadVec3(values, next);
      temp_pose[node].s = a + alpha * (b - a); // Assignment, not addition!
    }
  }

  // 3. Accumulate this specific animation's final pose into the global character pose
  for (int i = 0; i < instance.core_instance.nodes->size(); ++i) {
    instance.core_instance.current_locals[i].t += temp_pose[i].t * weight;
    instance.core_instance.current_locals[i].s += temp_pose[i].s * weight;

    // NLerp for quaternions
    if (instance.core_instance.current_locals[i].r.Dot(temp_pose[i].r) < 0.0f) {
      instance.core_instance.current_locals[i].r -= temp_pose[i].r * weight;
    } else {
      instance.core_instance.current_locals[i].r += temp_pose[i].r * weight;
    }
  }
}

void Animator::InitLocalsCharacter(CharacterInstance& instance, const Scene::CoreRig& rig) {
  for (int i = 0; i < instance.core_instance.nodes->size(); ++i) {
    instance.core_instance.current_locals_default[i] = (*instance.core_instance.nodes)[i]->local_transform;
    instance.core_instance.current_locals[i].t = JPH::Vec3::sZero();
    instance.core_instance.current_locals[i].s = JPH::Vec3::sZero();
    instance.core_instance.current_locals[i].r = JPH::Quat(0.0f, 0.0f, 0.0f, 0.0f);
  }

  // 3. Evaluate Blend Weights
  float weight_idle = 0.0f;
  float weight_walk = 0.0f;
  float weight_run  = 0.0f;

  auto cur_speed = instance.character->GetSpeed();
  if (cur_speed <= Character::cWalkSpeed) {
    // Blending between Idle (0.0) and Walk (walk_speed)
    weight_walk = cur_speed / Character::cWalkSpeed;
    weight_idle = 1.0f - weight_walk;
  } else {
    // Blending between Walk (walk_speed) and Run (run_speed)
    weight_run  = (cur_speed - Character::cWalkSpeed)
      / (Character::cRunSpeed - Character::cWalkSpeed);
    weight_walk = 1.0f - weight_run;
  }

  CharacterAnimType idle_stand_type = CharacterAnimType::Idle;
  CharacterAnimType idle_crouch_type = CharacterAnimType::IdleCrouch;
  bool character_crouch = instance.character->IsCrouch();
  bool character_aiming = instance.character->IsAiming();

  if (character_aiming) {
    idle_stand_type = CharacterAnimType::IdleNoArm;
    idle_crouch_type = CharacterAnimType::IdleCrouchNoArm;
  }

  const auto& idle_stand_anim = rig.animations[instance.skin->mapping.at(idle_stand_type)];
  const auto& idle_crouch_anim = rig.animations[instance.skin->mapping.at(idle_crouch_type)];

  float raise_speed = 6.0f;
  float target_crouch_weight = character_crouch ? 1.0f : 0.0f;
  if (instance.crouch_weight < target_crouch_weight) {
    instance.crouch_weight = std::min(instance.crouch_weight + raise_speed * gDeltaTime, target_crouch_weight);
  } else if (instance.crouch_weight > target_crouch_weight) {
    instance.crouch_weight = std::max(instance.crouch_weight - raise_speed * gDeltaTime, target_crouch_weight);
  }

  bool is_crouch_transition = (character_crouch && instance.crouch_weight != 1.0f) ||
    (!character_crouch && instance.crouch_weight != 0.0f);

  if (weight_idle > 0.001f) {
    float idle_crouch_weight = weight_idle * instance.crouch_weight;
    float idle_stand_weight = weight_idle * (1.0f - instance.crouch_weight);
    if (idle_crouch_weight > 0.001f) EvaluateWeightedAnimation(instance, idle_crouch_anim, idle_crouch_weight);
    if (idle_stand_weight > 0.001f) EvaluateWeightedAnimation(instance, idle_stand_anim, idle_stand_weight);
  }

  auto input_dir = instance.character->GetMoveDirectionRaw();
  if (input_dir.GetZ() == 0.0f && input_dir.GetX() == 0.0f) {
    if (cur_speed > 0.001f) {
      input_dir = instance.prev_input_dir;
    }
  }
  auto dir_sideways = input_dir.GetZ();
  auto dir_forward = input_dir.GetX();

  if (instance.strafing_) {
    dir_sideways = (instance.prev_input_dir.GetZ() < 0.0f) ? -1.0f : 1.0f;
    dir_forward = 0.0f;
  } else {
    if (dir_forward == 0.0f) {
      dir_forward = 1.0f;
    }
    dir_sideways = 0.0f;
  }

  if (instance.crouch_weight != 0.0f) {
    CharacterBlendingPoses pose_crouch = cPoseCrouch;
    CharacterBlendingPoses pose_walk = cPoseWalk;
    float weight_crouch = (weight_walk + weight_run) * instance.crouch_weight;
    weight_walk = (weight_walk + weight_run) * (1.0f - instance.crouch_weight);
    if (dir_sideways == 1.0f) { // Changed to > 0.5f just to be safe against float precision
      if (weight_walk > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_walk.right)], weight_walk);
      if (weight_crouch > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_crouch.right)], weight_crouch);
    }
    else if (dir_sideways == -1.0f) {
      if (weight_walk > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_walk.left)], weight_walk);
      if (weight_crouch > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_crouch.left)], weight_crouch);
    }
    else {
      if (dir_forward > 0.5f) {
        if (weight_walk > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_walk.forward)], weight_walk);
        if (weight_crouch > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_crouch.forward)], weight_crouch);
      }
      else if (dir_forward < -0.5f) {
        if (weight_walk > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_walk.backward)], weight_walk);
        if (weight_crouch > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_crouch.backward)], weight_crouch);
      }
    }
  } else {
    CharacterBlendingPoses pose_walk = cPoseWalk;
    CharacterBlendingPoses pose_run = cPoseRun;
    if (dir_sideways == 1.0f) { // Changed to > 0.5f just to be safe against float precision
      if (weight_walk > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_walk.right)], weight_walk);
      if (weight_run  > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_run.right)], weight_run);
    }
    else if (dir_sideways == -1.0f) {
      if (weight_walk > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_walk.left)], weight_walk);
      if (weight_run  > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_run.left)], weight_run);
    }
    else {
      if (dir_forward > 0.5f) {
        if (weight_walk > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_walk.forward)], weight_walk);
        if (weight_run  > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_run.forward)], weight_run);
      }
      else if (dir_forward < -0.5f) {
        if (weight_walk > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_walk.backward)], weight_walk);
        if (weight_run  > 0.001f) EvaluateWeightedAnimation(instance, rig.animations[instance.skin->mapping.at(pose_run.backward)], weight_run);
      }
    }
  }
  for (int i = 0; i < instance.core_instance.nodes->size(); ++i) {
    instance.core_instance.current_locals[i].r = instance.core_instance.current_locals[i].r.Normalized();
  }
  instance.prev_input_dir = input_dir;
}

void Animator::InitLocalsWeapon(CoreInstance& instance, const Scene::CoreRig& rig) {
  for (int i = 0; i < instance.nodes->size(); ++i) {
    instance.current_locals[i] = (*instance.nodes)[i]->local_transform;
  }
  float current_time = instance.time;
  const auto& animation = rig.animations[instance.type];
  WrapTime(instance, animation.samplers[animation.channels[0].sampler].times);
  for (const auto& channel : animation.channels) {
    const auto& sampler = animation.samplers[channel.sampler];
    const auto& times = sampler.times;
    const auto& values = sampler.values;
    int node = channel.target_node;

    int frame = FindFrame(times, current_time);
    int next = std::min(frame + 1, int(times.size() - 1));
    float alpha = 0.0f;
    if (times[next] > times[frame]) {
      alpha = (current_time - times[frame]) / (times[next] - times[frame]);
    }

    if (channel.target_path == "translation") {
      auto a = ReadVec3(values, frame);
      auto b = ReadVec3(values, next);
      instance.current_locals[node].t = a + alpha * (b - a);
    } else if (channel.target_path == "rotation") {
      auto a = ReadQuat(values, frame);
      auto b = ReadQuat(values, next);
      instance.current_locals[node].r = a.SLERP(b, alpha).Normalized();
    } else if (channel.target_path == "scale") {
      auto a = ReadVec3(values, frame);
      auto b = ReadVec3(values, next);
      instance.current_locals[node].s = a + alpha * (b - a);
    }
  }
}

void Animator::ComputeGlobals(CoreInstance& instance) {
  std::vector<int> used_ids;
  auto dfs = [&](auto& self, const SceneNode* node, const JPH::Mat44& parent) -> void {
    auto id = node->node_id;
    used_ids.push_back(id);
    auto local = instance.current_locals[id].Matrix();
    instance.current_globals[id] = parent * local;
    for (auto child : (*instance.nodes)[id]->children) {
      self(self, child, instance.current_globals[id]);
    }
  };
  dfs(dfs, instance.root_node, JPH::Mat44::sIdentity());
}

void Animator::BuildJointMatrices(
  CoreInstance& instance, const Scene::CoreRig& rig) {
  // JPH::Mat44 invMesh = instance.root_node->global_transform.Inversed();
  //TODO: global_transform invalid, we don't have it in zones, right?
  // invMesh = JPH::Mat44::sIdentity();
  for (size_t i = 0; i < rig.joints.size(); ++i) {
    int node = rig.joints[i].node;
    instance.current_joint_matrices[i] = /*invMesh * */instance.current_globals[node] * rig.joints[i].inverseBind;
  }
}

JPH::Mat44 Animator::GetNodeGlobalTransformCharacter(
    uint32_t instance_id, int node_id) const {
  return instances_characters_[instance_id].core_instance.current_globals[node_id];
}

JPH::Mat44 Animator::GetNodeGlobalTransformWeapon(
    uint32_t instance_id, int node_id) const {
  return instances_weapons_[instance_id].core_instance.current_globals[node_id];
}
