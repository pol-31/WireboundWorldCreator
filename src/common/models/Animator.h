#ifndef WIREBOUNDWORLDCREATOR_ANIMATOR_H
#define WIREBOUNDWORLDCREATOR_ANIMATOR_H

#include <glad/glad.h>
#include <tiny_gltf.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "Scene.h"

class Animator {
 public:
  enum class CharacterType {
    kIdle,
    kIdleSitting,
    kWalk,
    kRun,
    kCrouch,
    kKick,
    kStunned,
    kJump,
    kFall,
    kSlide,
    kClimb,
    kThrow,
    kSwim,
    kNone,
  };

  enum class WeaponType {
    Idle,
    Shoot,
  };

  struct Instance {
    const Scene::ModelNode* root_node = nullptr;
    const std::vector<Scene::ModelNode*>* nodes = nullptr;
    const Scene::Skin* skin = nullptr;
    float time = 0.0f;
    bool is_looped = true;
    int type = 0;
    int bones_offset = 0;
    bool is_idle = false; // if true - it's over and keeps same position type::time
    bool is_dead = false; // removed instance, nodes & skin invalid, skip
    std::vector<JPH::Mat44> current_globals;
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

  size_t AddInstance(
      const Scene::ModelNode* root_node,
      const std::vector<Scene::ModelNode*>* nodes,
      const Scene::Skin* skin);

  void RemoveInstance(int id);

  [[nodiscard]] const Instance& GetInstance(int id) const noexcept {
    return instances_[id];
  }

  void Start(int instance_id, int type, bool looped);

  /// updates all animations
  void Update();

  JPH::Mat44 GetNodeGlobalTransform(uint32_t instance_id, int node_id) const;

 private:
  void Init();

  void DeInit();

  static std::vector<Scene::NodePose> InitLocals(
    Instance& instance);

  static std::vector<JPH::Mat44> ComputeGlobals(
    Instance& instance, const std::vector<Scene::NodePose>& locals);

  static std::vector<JPH::Mat44> BuildJointMatrices(
    Instance& instance, const std::vector<JPH::Mat44>& globals);

  void Clear();

  std::vector<Instance> instances_;
  float speed_ = 1.0f;
  GLuint ssbo_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_ANIMATOR_H
