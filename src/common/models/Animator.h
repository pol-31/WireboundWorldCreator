#ifndef WIREBOUNDWORLDCREATOR_ANIMATOR_H
#define WIREBOUNDWORLDCREATOR_ANIMATOR_H

#include <glad/glad.h>
#include <tiny_gltf.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

enum class HumanAnimation {
  kCrouch,
  kFall,
  kIdle,
  kIdleSitting,
  kJump,
  kKick,
  kRun,
  kStunned,
  kT,
  kThrow,
  kWalk,
  kNone,
};

enum class FpvAnimation {
  kCrouch,
  kFall,
  kIdle,
  kIdleSitting,
  kJump,
  kKick,
  kRun,
  kStunned,
  kT,
  kThrow,
  kWalk,
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

  Animator(tinygltf::TinyGLTF& loader, const GLuint& ubo);

  void Load(std::string_view path);

  /// throw data to ubo, buffer, so need to call Render() after.
  /// returns was_looped
  bool UpdateUbo(int id, float& time);

  bool UpdateUbo(HumanAnimation animation, float& time) {
    if (animation == HumanAnimation::kNone) {
      SetZeroUbo();
      return false;
    }
    return UpdateUbo(static_cast<int>(animation), time);
  }

  bool UpdateUbo(FpvAnimation animation, float& time) {
    if (animation == FpvAnimation::kNone) {
      SetZeroUbo();
      return false;
    }
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

  tinygltf::TinyGLTF& loader_;
  tinygltf::Model model_;  // skin + animations
  std::vector<glm::mat4> joints_zero_;

  float speed_ = 1.0f;

  const GLuint& ubo_;
  Skin skin_;
};

#endif  // WIREBOUNDWORLDCREATOR_ANIMATOR_H
