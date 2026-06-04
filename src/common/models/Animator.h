#ifndef WIREBOUNDWORLDCREATOR_ANIMATOR_H
#define WIREBOUNDWORLDCREATOR_ANIMATOR_H

#include <glad/glad.h>
#include <tiny_gltf.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

class Animator {
 public:
  enum class Type {
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

  struct Instance {
    float time = 0.0f;
    bool is_looped = true;
    Type type = Type::kIdle;
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
  Animator(tinygltf::Model model);

  ~Animator();

  /// return instance_id
  size_t AddInstance() {
    instances_.push_back({});
    return instances_.size() - 1;
  }

  const Instance& GetInstance(int id) const noexcept {
    return instances_[id];
  }

  void Start(int instance_id, Type type);

  /// updates all animations
  void Update();

 private:
  void LoadSkin(const tinygltf::Model& model, Skin& skin);

  void InitLocalPose(const tinygltf::Model& model,
                     std::vector<NodePose>& localPose);

  float ApplyAnimation(const tinygltf::Model& model, int animIndex, float time,
                       std::vector<NodePose>& localPose);

  void ComputeGlobals(const tinygltf::Model& model,
                      std::vector<NodePose>& localPose,
                      std::vector<glm::mat4>& globalPose);

  void BuildJointMatrices(const Skin& skin,
                          const std::vector<glm::mat4>& globalPose,
                          const glm::mat4& meshGlobal,
                          std::vector<glm::mat4>& out);

  void Clear();

  std::vector<Instance> instances_;

  tinygltf::Model model_;
  std::vector<glm::mat4> joints_zero_;
  float speed_ = 1.0f;

  GLuint ssbo_ = 0;
  Skin skin_;
};

#endif  // WIREBOUNDWORLDCREATOR_ANIMATOR_H
