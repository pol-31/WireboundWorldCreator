#include "Animator.h"

#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../../io/Window.h"

const int Animator::gMaxBones = 1000;

int FindFrame(const std::vector<float>& times, float t) {
  if (t <= times[0]) return 0;

  auto count = times.size();
  for (int i = 0; i < count - 1; ++i) {
    if (t < times[i + 1]) return i;
  }

  return count - 2;  // last valid segment
}

void WrapTime(Animator::Instance& instance, const std::vector<float>& times) {
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

size_t Animator::AddInstance(
      const Scene::ModelNode* root_node,
      const std::vector<Scene::ModelNode*>* nodes,
      const Scene::Skin* skin) {
  for (int i = 0; i < instances_.size(); ++i) {
    auto& instance = instances_[i];
    if (instance.is_dead) {
      instance = Instance(); // resets is_dead as well
      instance.root_node = root_node;
      instance.nodes = nodes;
      instance.skin = skin;
      return i;
    }
  }
  Instance instance;
  instance.root_node = root_node;
  instance.nodes = nodes;
  instance.skin = skin;
  instances_.push_back(instance);
  return instances_.size() - 1;
}

void Animator::RemoveInstance(int id) {
  instances_[id].is_dead = true;
}

void Animator::Start(int instance_id, int type, bool looped) {
  auto& data = instances_[instance_id];
  data.time = 0.0f;
  data.is_looped = looped; // TODO: separate
  data.type = type;
  data.is_idle = false;
}

void Animator::Update() {
  std::vector<JPH::Mat44> all_jointMatrices;
  for (auto& instance : instances_) {
    if (instance.is_dead) {
      return;
    }
    if (!instance.is_idle) {
      instance.time += gDeltaTime * speed_;
    }
    instance.bones_offset = all_jointMatrices.size();
    auto locals = InitLocals(instance);
    instance.current_globals = ComputeGlobals(instance, locals);
    auto joint_matrices = BuildJointMatrices(instance, instance.current_globals);
    all_jointMatrices.insert(all_jointMatrices.end(), joint_matrices.begin(), joint_matrices.end());
  }

  if (all_jointMatrices.size() > gMaxBones) {
    std::cerr << "too much bones for ssbo" << std::endl;
    all_jointMatrices.resize(gMaxBones);
  }
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
  // glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                  // all_jointMatrices.size() * sizeof(glm::mat4),
                  // all_jointMatrices.data());
  glBufferData(GL_SHADER_STORAGE_BUFFER,
             all_jointMatrices.size() * sizeof(glm::mat4),
             all_jointMatrices.data(),
             GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

std::vector<Scene::NodePose> ReadInitLocals(
    const std::vector<Scene::ModelNode*>& nodes) {
  std::vector<Scene::NodePose> locals(nodes.size());
  for (int i = 0; i < nodes.size(); ++i) {
    locals[i] = nodes[i]->local_transform;
  }
  return locals;
}

JPH::Vec3 ReadVec3(const std::vector<float>& v, int index)
{
  return JPH::Vec3(
      v[index * 3 + 0],
      v[index * 3 + 1],
      v[index * 3 + 2]);
}

JPH::Quat ReadQuat(const std::vector<float>& v, int index)
{
  return JPH::Quat(
      v[index * 4 + 0],
      v[index * 4 + 1],
      v[index * 4 + 2],
      v[index * 4 + 3]);
}

std::vector<Scene::NodePose> Animator::InitLocals(Instance& instance) {
  auto locals = ReadInitLocals(*instance.nodes);
  const auto& anim = instance.skin->animations[instance.type];
  float current_time = instance.time;
  for (const auto& channel : anim.channels) {
    const auto& sampler = anim.samplers[channel.sampler];
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
      locals[node].t = a + alpha * (b - a);
    } else if (channel.target_path == "rotation") {
      auto a = ReadQuat(values, frame);
      auto b = ReadQuat(values, next);
      locals[node].r = a.SLERP(b, alpha).Normalized();
    } else if (channel.target_path == "scale") {
      auto a = ReadVec3(values, frame);
      auto b = ReadVec3(values, next);
      locals[node].s = a + alpha * (b - a);
    }
  }
  return locals;
}

std::vector<JPH::Mat44> Animator::ComputeGlobals(
    Instance& instance, const std::vector<Scene::NodePose>& locals) {
  std::vector<JPH::Mat44>globals(instance.nodes->size());
  std::function<void(const Scene::ModelNode*, const JPH::Mat44&)> dfs =
      [&](const Scene::ModelNode* node, const JPH::Mat44& parent) {
        auto id = node->node_id;
        auto local = locals[id].Matrix();
        globals[id] = parent * local;
        for (auto child : (*instance.nodes)[id]->children) {
          dfs(child, globals[id]);
        }
      };
  dfs(instance.root_node, JPH::Mat44::sIdentity());
  return globals;
}

std::vector<JPH::Mat44> Animator::BuildJointMatrices(
    Instance& instance, const std::vector<JPH::Mat44>& globals) {
  std::vector<JPH::Mat44> out(instance.skin->joints.size());
  JPH::Mat44 invMesh = instance.root_node->global_transform.Inversed();
  for (size_t i = 0; i < instance.skin->joints.size(); ++i) {
    int node = instance.skin->joints[i].node;
    out[i] = invMesh * globals[node] * instance.skin->joints[i].inverseBind;
  }
  return out;
}

JPH::Mat44 Animator::GetNodeGlobalTransform(uint32_t instance_id, int node_id) const {
  return instances_[instance_id].current_globals[node_id];
}
