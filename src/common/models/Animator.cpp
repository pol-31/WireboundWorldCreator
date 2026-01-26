#include "Animator.h"

#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../../io/Window.h"

const int Animator::gMaxBones = 100;

int FindFrame(int count, const float* times, float t) {
  if (t <= times[0]) return 0;

  for (int i = 0; i < count - 1; ++i) {
    if (t < times[i + 1]) return i;
  }

  return count - 2;  // last valid segment
}

glm::mat4 LocalMatrix(const Animator::NodePose& p) {
  return glm::translate(glm::mat4(1), p.t) * glm::mat4_cast(p.r) *
         glm::scale(glm::mat4(1), p.s);
}

const float* GetFloatData(const tinygltf::Model& model,
                          const tinygltf::Accessor& acc) {
  const auto& view = model.bufferViews[acc.bufferView];
  const auto& buffer = model.buffers[view.buffer];

  return reinterpret_cast<const float*>(buffer.data.data() + view.byteOffset +
                                        acc.byteOffset);
}

float WrapTime(float t, const float* times, int count) {
  float start = times[0];
  float end = times[count - 1];
  float duration = end - start;

  if (duration <= 0.0f) return start;

  // keep phase, support large t
  return start + std::fmod(t - start, duration);
}

Animator::Animator(tinygltf::TinyGLTF& loader, const GLuint& ubo)
    : loader_(loader), speed_(0.5f), ubo_(ubo) {}

void Animator::Load(std::string_view path) {
  std::string err, warn;
  bool res = loader_.LoadASCIIFromFile(&model_, &err, &warn, path.data());
  if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
  if (!err.empty()) std::cout << "ERR: " << err << std::endl;

  if (!res)
    throw "Failed to load glTF";
  else
    std::cout << "Loaded glTF: " << path << std::endl;

  if (model_.skins.size() != 1 || model_.animations.size() == 0) {
    throw "wrong animation data";
  }
  // buffers
  std::vector<NodePose> localPose;
  std::vector<glm::mat4> globalPose;
  std::vector<glm::mat4> jointMatrices;

  LoadSkin(model_, skin_);
  InitLocalPose(model_, localPose);

  ComputeGlobals(model_, localPose, globalPose);
  glm::mat4 meshGlobal{1.0f};  // TODO: wrong
  BuildJointMatrices(skin_, globalPose, meshGlobal, jointMatrices);

  joints_zero_ = std::vector(gMaxBones, glm::mat4(1.0f));
  SetZeroUbo();
}

void Animator::SetZeroUbo() {
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, joints_zero_.size() * sizeof(glm::mat4),
                  joints_zero_.data());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

/// throw data to ubo, buffer, so need to call Render() afterwards
bool Animator::UpdateUbo(int id, float& time) {
  // buffers
  std::vector<NodePose> localPose;
  std::vector<glm::mat4> globalPose;
  std::vector<glm::mat4> jointMatrices;

  time += gDeltaTime * speed_;
  InitLocalPose(model_, localPose);
  float mod_time = ApplyAnimation(model_, id, time, localPose);
  bool started_over = time > mod_time;
  if (started_over) {
    time = mod_time;
  }
  ComputeGlobals(model_, localPose, globalPose);
  glm::mat4 meshGlobal{1.0f};  // TODO: wrong
  BuildJointMatrices(skin_, globalPose, meshGlobal, jointMatrices);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0,
                  jointMatrices.size() * sizeof(glm::mat4),
                  jointMatrices.data());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  return started_over;
}

void Animator::LoadSkin(const tinygltf::Model& model, Skin& skin) {
  const tinygltf::Skin& gltfSkin = model.skins[0];

  skin.skeletonRoot = gltfSkin.skeleton;

  skin.joints.resize(gltfSkin.joints.size());

  // Load inverse bind matrices
  const tinygltf::Accessor& acc = model.accessors[gltfSkin.inverseBindMatrices];
  const tinygltf::BufferView& bv = model.bufferViews[acc.bufferView];
  const tinygltf::Buffer& buf = model.buffers[bv.buffer];

  const float* data =
      reinterpret_cast<const float*>(&buf.data[bv.byteOffset + acc.byteOffset]);

  for (size_t i = 0; i < skin.joints.size(); ++i) {
    skin.joints[i].node = gltfSkin.joints[i];
    skin.joints[i].inverseBind = glm::make_mat4(data + i * 16);
  }
}

void Animator::InitLocalPose(const tinygltf::Model& model,
                             std::vector<NodePose>& localPose) {
  localPose.resize(model.nodes.size());

  for (size_t i = 0; i < model.nodes.size(); ++i) {
    const auto& n = model.nodes[i];

    if (n.translation.size() == 3) {
      localPose[i].t =
          glm::vec3(float(n.translation[0]), float(n.translation[1]),
                    float(n.translation[2]));
    } else {
      localPose[i].t = glm::vec3(0.0f);
    }

    if (n.rotation.size() == 4) {
      localPose[i].r = glm::quat(float(n.rotation[3]),   // w
                                 float(n.rotation[0]),   // x
                                 float(n.rotation[1]),   // y
                                 float(n.rotation[2]));  // z
    } else {
      localPose[i].r = glm::quat(1, 0, 0, 0);
    }

    if (n.scale.size() == 3) {
      localPose[i].s =
          glm::vec3(float(n.scale[0]), float(n.scale[1]), float(n.scale[2]));
    } else {
      localPose[i].s = glm::vec3(1.0f);
    }
  }
}

float Animator::ApplyAnimation(const tinygltf::Model& model, int animIndex,
                               float time, std::vector<NodePose>& localPose) {
  const auto& anim = model.animations[animIndex];

  for (const auto& channel : anim.channels) {
    const auto& sampler = anim.samplers[channel.sampler];
    int node = channel.target_node;

    const tinygltf::Accessor& inAcc = model.accessors[sampler.input];
    const tinygltf::Accessor& outAcc = model.accessors[sampler.output];

    const float* times = GetFloatData(model, inAcc);
    const float* values = GetFloatData(model, outAcc);

    time = WrapTime(time, times, inAcc.count);

    int frame = FindFrame(inAcc.count, times, time);
    int next = std::min(frame + 1, int(inAcc.count - 1));

    float alpha = (time - times[frame]) / (times[next] - times[frame]);

    if (channel.target_path == "translation") {
      glm::vec3 a = glm::make_vec3(values + frame * 3);
      glm::vec3 b = glm::make_vec3(values + next * 3);
      localPose[node].t = glm::mix(a, b, alpha);
    }

    if (channel.target_path == "rotation") {
      glm::quat a = glm::make_quat(values + frame * 4);
      glm::quat b = glm::make_quat(values + next * 4);
      localPose[node].r = glm::normalize(glm::slerp(a, b, alpha));
    }

    if (channel.target_path == "scale") {
      glm::vec3 a = glm::make_vec3(values + frame * 3);
      glm::vec3 b = glm::make_vec3(values + next * 3);
      localPose[node].s = glm::mix(a, b, alpha);
    }
  }
  return time;
}

void Animator::ComputeGlobals(const tinygltf::Model& model,
                              std::vector<NodePose>& localPose,
                              std::vector<glm::mat4>& globalPose) {
  globalPose.resize(model.nodes.size());

  std::function<void(int, const glm::mat4&)> dfs =
      [&](int node, const glm::mat4& parent) {
        glm::mat4 local = LocalMatrix(localPose[node]);
        globalPose[node] = parent * local;

        for (int child : model.nodes[node].children)
          dfs(child, globalPose[node]);
      };

  // Scene roots
  for (int root : model.scenes[model.defaultScene].nodes)
    dfs(root, glm::mat4(1));
}

void Animator::BuildJointMatrices(const Skin& skin,
                                  const std::vector<glm::mat4>& globalPose,
                                  const glm::mat4& meshGlobal,
                                  std::vector<glm::mat4>& out) {
  out.resize(skin.joints.size());

  glm::mat4 invMesh = glm::inverse(meshGlobal);

  for (size_t i = 0; i < skin.joints.size(); ++i) {
    int node = skin.joints[i].node;
    out[i] = invMesh * globalPose[node] * skin.joints[i].inverseBind;
  }
}
