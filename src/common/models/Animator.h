#ifndef WIREBOUNDWORLDCREATOR_ANIMATOR_H
#define WIREBOUNDWORLDCREATOR_ANIMATOR_H

#include <glad/glad.h>
#include <tiny_gltf.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

enum class Animation {
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

class Animator {
 public:
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

  /// throw data to ubo, buffer, so need to call Render() after.
  /// returns was_looped
  bool UpdateUbo(int id, float& time);

  bool UpdateUbo(Animation animation, float& time) {
    return UpdateUbo(static_cast<int>(animation), time);
  }

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

 private:
  void SetZeroUbo();

  tinygltf::Model model_;
  std::vector<glm::mat4> joints_zero_;
  float speed_ = 1.0f;

  GLuint ubo_;
  Skin skin_;
};

#endif  // WIREBOUNDWORLDCREATOR_ANIMATOR_H
