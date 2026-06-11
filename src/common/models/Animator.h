#ifndef WIREBOUNDWORLDCREATOR_ANIMATOR_H
#define WIREBOUNDWORLDCREATOR_ANIMATOR_H

#include <glad/glad.h>
#include <tiny_gltf.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

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
    float time = 0.0f;
    bool is_looped = true;
    int type = 0;
    int bones_offset = 0;
  };

  static const int gMaxBones;

  struct Joint {
    int node;               // index into model.nodes
    glm::mat4 inverseBind;  // from glTF
  };

  struct Skin {
    int skeletonRoot;  // node index or -1
    std::vector<Joint> joints;
  };

  struct NodePose {
    glm::vec3 t;
    glm::quat r;
    glm::vec3 s;
  };

  Animator() = default;
  Animator(tinygltf::Model model_character, tinygltf::Model model_weapon);

  ~Animator();

  size_t AddInstanceCharacter() {
    rigs_[0].instances_.push_back({});
    return rigs_[0].instances_.size() - 1;
  }

  const Instance& GetInstanceCharacter(int id) const noexcept {
    return rigs_[0].instances_[id];
  }

  size_t AddInstanceWeapon() {
    rigs_[1].instances_.push_back({});
    return rigs_[1].instances_.size() - 1;
  }

  const Instance& GetInstanceWeapon(int id) const noexcept {
    return rigs_[1].instances_[id];
  }

  void Start(int instance_id, CharacterType type, bool looped) {
    Start(instance_id, static_cast<int>(type), 0, looped);
  }
  void Start(int instance_id, WeaponType type, bool looped) {
    Start(instance_id, static_cast<int>(type), 1, looped);
  }

  /// updates all animations
  void Update();

 private:
  void Start(int instance_id, int type, int rig_id, bool looped);

  static void LoadSkin(const tinygltf::Model& model, Skin& skin);

  static void InitLocalPose(const tinygltf::Model& model,
                     std::vector<NodePose>& localPose);

  static float ApplyAnimation(const tinygltf::Model& model, int animIndex, float time,
                       std::vector<NodePose>& localPose);

  static void ComputeGlobals(const tinygltf::Model& model,
                      std::vector<NodePose>& localPose,
                      std::vector<glm::mat4>& globalPose);

  static void BuildJointMatrices(const Skin& skin,
                          const std::vector<glm::mat4>& globalPose,
                          const glm::mat4& meshGlobal,
                          std::vector<glm::mat4>& out);

  void Clear();

  struct Rig {
    std::vector<Instance> instances_;
    tinygltf::Model model_;
    Skin skin_;
  };

  std::vector<Rig> rigs_;

  float speed_ = 1.0f;
  GLuint ssbo_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_ANIMATOR_H
