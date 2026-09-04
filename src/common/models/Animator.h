#ifndef WIREBOUNDWORLDCREATOR_ANIMATOR_H
#define WIREBOUNDWORLDCREATOR_ANIMATOR_H

#include <glad/glad.h>
#include <tiny_gltf.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "Scene.h"

class Character;

class Animator {
 public:
  struct CoreInstance {
    const Scene::ModelNode* root_node = nullptr;
    const std::vector<Scene::ModelNode*>* nodes = nullptr;
    float time = 0.0f;
    bool is_looped = true;
    int type = 0;
    int bones_offset = 0;
    bool is_alive = true;
    bool is_idle = false; // if true - it's over and keeps same position type::time
    JPH::Mat44 identity = JPH::Mat44::sIdentity(); //  rutch for invalid models
    std::vector<Scene::NodePose> current_locals;
    std::vector<Scene::NodePose> current_locals_default; // for blending
    std::vector<JPH::Mat44> current_globals;
    std::vector<JPH::Mat44> current_joint_matrices;
  };

  struct CharacterInstance {
    CoreInstance core_instance;
    bool has_pistol = false;
    const Scene::CharacterRig* skin = nullptr;
    const Character* character = nullptr;
    // final render yaw (bone upper body already blended INTO the rig globals)
    float body_yaw_ = 0.0f;
    bool strafing_ = false;
    float armed_weight = 0.0f; // 0 - disarmed
    float crouch_weight = 0.0f; // 0 - stand
    JPH::Vec3 prev_input_dir = JPH::Vec3::sZero(); // in case no input, but inertia
  };

  struct WeaponInstance {
    CoreInstance core_instance;
    const Scene::WeaponRig* skin = nullptr;
  };

  enum class AnimationLayerFilter {
    AllNodes,
    LowerBodyOnly,
    UpperBodyOnly
  };

  static const int gMaxBones;

  Animator() {
    Init();
  }

  ~Animator() {
    DeInit();
  }

  Animator(const Animator& animator) = delete;
  Animator(Animator&& animator) = delete;
  Animator& operator=(const Animator& animator) = delete;
  Animator& operator=(Animator&& animator) = delete;

  size_t AddInstanceCharacter(
      const Scene::ModelNode* root_node,
      const std::vector<Scene::ModelNode*>* nodes,
      const Scene::CharacterRig* skin,
      const Character* character);

  size_t AddInstanceWeapon(
      const Scene::ModelNode* root_node,
      const std::vector<Scene::ModelNode*>* nodes,
      const Scene::WeaponRig* skin);

  void RemoveInstanceCharacter(int id);
  void RemoveInstanceWeapon(int id);

  void StartCharacter(int instance_id, CharacterAnimType type, bool looped);
  void StartWeapon(int instance_id, WeaponAnimType type, bool looped);

  [[nodiscard]] const CharacterInstance& GetInstanceCharacter(int id) const noexcept {
    return instances_characters_[id];
  }
  [[nodiscard]] const WeaponInstance& GetInstanceWeapon(int id) const noexcept {
    return instances_weapons_[id];
  }

  /// updates all animations
  void Update(bool skip = false);

  JPH::Mat44 GetNodeGlobalTransformCharacter(
    uint32_t instance_id, int node_id) const;
  JPH::Mat44 GetNodeGlobalTransformWeapon(
    uint32_t instance_id, int node_id) const;

 private:
  void Init();

  void DeInit();

  static void EvaluateWeightedAnimation(
    CharacterInstance& instance, const Scene::Animation& animation, float weight);

  void ApplyDeltas(CharacterInstance& instance, JPH::Vec3 prev_move_dir);

  static void InitLocalsCharacter(CharacterInstance& instance, const Scene::CoreRig& rig);
  static void InitLocalsWeapon(CoreInstance& instance, const Scene::CoreRig& rig);

  static void ComputeGlobals(CoreInstance& instance);

  static void BuildJointMatrices(CoreInstance& instance, const Scene::CoreRig& rig);

  void Clear();

  void CompensateLowerToUpperSpines(CharacterInstance& instance);

  std::vector<WeaponInstance> instances_weapons_;
  std::vector<CharacterInstance> instances_characters_;
  float speed_ = 1.0f;
  GLuint ssbo_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_ANIMATOR_H
