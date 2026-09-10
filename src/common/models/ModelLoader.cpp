#include "ModelLoader.h"

#include <array>

#include <stb_image.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Animator.h"
#include "ModelLoader.h"

const int materialWidth = 2048;
const int materialHeight = 2048;

JPH::Vec3 GetNodeTranslation(const tinygltf::Node& node) {
  auto position = JPH::Vec3::sZero();
  if (node.translation.size() == 3) {
    position = JPH::Vec3(node.translation[0], node.translation[1],
                            node.translation[2]);
  }
  return position;
}

JPH::Vec3 GetNodeScale(const tinygltf::Node& node) {
  auto scale = JPH::Vec3::sOne();
  if (node.scale.size() == 3) {
    scale = JPH::Vec3(node.scale[0], node.scale[1], node.scale[2]);
  }
  return scale;
}

JPH::Quat GetNodeRotation(const tinygltf::Node& node) {
  auto rotation = JPH::Quat::sIdentity();
  if (node.rotation.size() == 4) {
    rotation = JPH::Quat(node.rotation[0], node.rotation[1],
                         node.rotation[2], node.rotation[3]);
  }
  return rotation;
}

bool LoadImageData(tinygltf::Image* image, const int image_idx,
                   std::string* err, std::string* warn, int req_width,
                   int req_height, const unsigned char* bytes, int size,
                   void* user_data) {
  (void)image;
  (void)image_idx;
  (void)err;
  (void)warn;
  (void)req_width;
  (void)req_height;
  (void)bytes;
  (void)size;
  (void)user_data;
  return true;
}

inline void* ByteOffset(std::size_t offset) noexcept {
  return reinterpret_cast<void*>(static_cast<std::uintptr_t>(offset));
}

ModelLoader::ModelLoader() {
  loader_.SetImageLoader(LoadImageData, nullptr);
}

ModelLoader::~ModelLoader() {
  // for (auto& model : models_) {
  //   for (auto& mesh : model->primitives) {
  //     glDeleteVertexArrays(1, &mesh.vao);
  //   }
  // }
}

std::map<std::string, CharacterAnimType> character_anim_map_str_to_enum = {
    {"idle", CharacterAnimType::Idle},
    {"idle_no_arm", CharacterAnimType::IdleNoArm},
    {"idle_climb", CharacterAnimType::Climb},
    {"idle_crouch", CharacterAnimType::IdleCrouch},
    {"idle_crouch_no_arm", CharacterAnimType::IdleCrouchNoArm},
    {"idle_fall", CharacterAnimType::Fall},
    {"idle_holster", CharacterAnimType::Holster},
    {"idle_hurt", CharacterAnimType::Hurt},
    {"idle_jump", CharacterAnimType::Jump},
    {"idle_kick", CharacterAnimType::Kick},
    {"idle_move_jump", CharacterAnimType::MoveJump},
    {"idle_reload", CharacterAnimType::Reload},
    {"idle_shoot", CharacterAnimType::Shoot},
    {"idle_sitting", CharacterAnimType::IdleSitting},
    {"idle_slide", CharacterAnimType::Slide},
{"__walk_forward", CharacterAnimType::WalkForward},
{"__walk_backward", CharacterAnimType::WalkBackward},
{"__walk_left", CharacterAnimType::WalkLeft},
{"__walk_right", CharacterAnimType::WalkRight},
{"__run_forward", CharacterAnimType::RunForward},
{"__run_backward", CharacterAnimType::RunBackward},
{"__run_left", CharacterAnimType::RunLeft},
{"__run_right", CharacterAnimType::RunRight},
{"__crouch_forward", CharacterAnimType::CrouchForward},
{"__crouch_backward", CharacterAnimType::CrouchBackward},
{"__crouch_left", CharacterAnimType::CrouchLeft},
{"__crouch_right", CharacterAnimType::CrouchRight},
{"poses_idle", CharacterAnimType::PosesIdle},
{"poses_pistol", CharacterAnimType::PosesPistol},
};
// poses order: center down left right up

std::map<std::string, WeaponAnimType> weapon_anim_map_str_to_enum = {
  {"Idle", WeaponAnimType::Idle},
  {"Shoot", WeaponAnimType::Shoot},
  {"Reload", WeaponAnimType::Reload},
};

std::map<CharacterAnimType, int> CreateCharacterAnimationMapping(
  const std::vector<Scene::Animation>& animations) {
  std::map<CharacterAnimType, int> map;
  // inside the gltf they stored exactly like in vector_animations,
  // so we take EVERY animation's name and bind enum by name to id
  for (int id = 0; id < animations.size(); ++id) {
    const auto& name = animations[id].name;
    auto it = character_anim_map_str_to_enum.find(name);
    if (it == character_anim_map_str_to_enum.end()) {
      throw std::runtime_error("character animations name map inconsistency");
    }
    map[it->second] = id; // key[enum] = value[id]
  }
  return map;
}

std::map<WeaponAnimType, int> CreateWeaponAnimationMapping(
  const std::vector<Scene::Animation>& animations) {
  std::map<WeaponAnimType, int> map;
  for (int id = 0; id < animations.size(); ++id) {
    const auto& name = animations[id].name;
    auto it = weapon_anim_map_str_to_enum.find(name);
    if (it == weapon_anim_map_str_to_enum.end()) {
      throw std::runtime_error("weapon animations name map inconsistency");
    }
    map[it->second] = id;
  }
  return map;
}

static JPH::Vec3 ReadVec3(const std::vector<float>& v, int index) {
  return JPH::Vec3(
      v[index * 3 + 0],
      v[index * 3 + 1],
      v[index * 3 + 2]);
}
static JPH::Vec3 ReadVec3(const std::vector<double>& v, int index) {
  return JPH::Vec3(
      static_cast<float>(v[index * 3 + 0]),
      static_cast<float>(v[index * 3 + 1]),
      static_cast<float>(v[index * 3 + 2]));
}

static JPH::Quat ReadQuat(const std::vector<float>& v, int index) {
  return JPH::Quat(
      v[index * 4 + 0],
      v[index * 4 + 1],
      v[index * 4 + 2],
      v[index * 4 + 3]);
}
static JPH::Quat ReadQuat(const std::vector<double>& v, int index) {
  return JPH::Quat(
      static_cast<float>(v[index * 4 + 0]),
      static_cast<float>(v[index * 4 + 1]),
      static_cast<float>(v[index * 4 + 2]),
      static_cast<float>(v[index * 4 + 3]));
}

// void ReadPose(const Scene::Animation& anim,
//   std::vector<Scene::NodePose>& locals, CharacterPoseDir pose) {
//   for (const auto& channel : anim.channels) {
//     const auto& sampler = anim.samplers[channel.sampler];
//     const auto& values = sampler.values;
//     int node = channel.target_node;
//     auto frame = static_cast<int>(pose);
//     //frame = 0;
//     if (channel.target_path == "translation") {
//       locals[node].t = ReadVec3(values, frame);
//     } else if (channel.target_path == "rotation") {
//       locals[node].r = ReadQuat(values, frame).Normalized();
//     } else if (channel.target_path == "scale") {
//       locals[node].s = ReadVec3(values, frame);
//     }
//   }
// }

void ReadPose(const Scene::Animation& anim,
  std::vector<SceneNodePose>& locals, CharacterPoseDir pose) {
  for (const auto& channel : anim.channels) {
    const auto& sampler = anim.samplers[channel.sampler];
    const auto& values = sampler.values;
    int node = channel.target_node;
    int frame = static_cast<int>(pose);

    if (channel.target_path == "translation") {
      // 3 floats per translation keyframe
      int max_frame = (values.size() / 3) - 1;
      int safe_frame = std::max(0, std::min(frame, max_frame));
      locals[node].t = ReadVec3(values, safe_frame);

    } else if (channel.target_path == "rotation") {
      // 4 floats per rotation keyframe
      int max_frame = (values.size() / 4) - 1;
      int safe_frame = std::max(0, std::min(frame, max_frame));
      locals[node].r = ReadQuat(values, safe_frame).Normalized();

    } else if (channel.target_path == "scale") {
      // 3 floats per scale keyframe
      int max_frame = (values.size() / 3) - 1;
      int safe_frame = std::max(0, std::min(frame, max_frame));
      locals[node].s = ReadVec3(values, safe_frame);
    }
  }
}

std::vector<SceneNodePose> CalculateDelta(
  const std::vector<SceneNodePose>& starting_locals,
  const std::vector<SceneNodePose>& center_locals,
  const Scene::Animation& anim, CharacterPoseDir pose) {
  std::vector<SceneNodePose> direction_locals = starting_locals;
  ReadPose(anim, direction_locals, pose);
  for (int i = 0; i < center_locals.size(); ++i) {
    direction_locals[i].t -= center_locals[i].t;
    direction_locals[i].s = direction_locals[i].s / center_locals[i].s;
    direction_locals[i].r = direction_locals[i].r * center_locals[i].r.Inversed();
  }
  return direction_locals;
}

void LoadDeltas(
  const std::map<CharacterAnimType, int>& mapping,
  const std::vector<Scene::Animation>& animations,
  const std::vector<SceneNode*>& nodes,
  Scene::PoseDeltas& default_deltas,
  Scene::PoseDeltas& pistol_deltas) {
  std::vector<SceneNodePose> starting_locals(nodes.size());
  for (int i = 0; i < nodes.size(); ++i) {
    starting_locals[i] = nodes[i]->local_transform;
  }

  std::vector<SceneNodePose> center_locals = starting_locals;
  const auto& poses_idle = animations[mapping.at(CharacterAnimType::PosesIdle)];
  ReadPose(poses_idle, center_locals, CharacterPoseDir::Center);
  default_deltas.center = CalculateDelta(starting_locals, starting_locals,
    poses_idle, CharacterPoseDir::Center);
  default_deltas.left = CalculateDelta(starting_locals, center_locals,
    poses_idle, CharacterPoseDir::Left);
  default_deltas.right = CalculateDelta(starting_locals, center_locals,
    poses_idle, CharacterPoseDir::Right);
  default_deltas.up = CalculateDelta(starting_locals, center_locals,
    poses_idle, CharacterPoseDir::Up);
  default_deltas.down = CalculateDelta(starting_locals, center_locals,
    poses_idle, CharacterPoseDir::Down);

  center_locals = starting_locals;
  const auto& poses_pistol = animations[mapping.at(CharacterAnimType::PosesPistol)];
  ReadPose(poses_pistol, center_locals, CharacterPoseDir::Center);
  pistol_deltas.center = CalculateDelta(starting_locals, starting_locals,
    poses_pistol, CharacterPoseDir::Center);
  pistol_deltas.left = CalculateDelta(starting_locals, center_locals,
    poses_pistol, CharacterPoseDir::Left);
  pistol_deltas.right = CalculateDelta(starting_locals, center_locals,
    poses_pistol, CharacterPoseDir::Right);
  pistol_deltas.up = CalculateDelta(starting_locals, center_locals,
    poses_pistol, CharacterPoseDir::Up);
  pistol_deltas.down = CalculateDelta(starting_locals, center_locals,
    poses_pistol, CharacterPoseDir::Down);
}

Scene::CoreRig ModelLoader::LoadCoreRig(const tinygltf::Model& model) {
  const tinygltf::Skin& gltfSkin = model.skins[0];
  Scene::CoreRig skin;
  skin.skeletonRoot = model.scenes[0].nodes[0]; // skin.skeleton invalid idk
  skin.joints.resize(gltfSkin.joints.size());

  // Load inverse bind matrices
  const tinygltf::Accessor& acc = model.accessors[gltfSkin.inverseBindMatrices];
  const tinygltf::BufferView& bv = model.bufferViews[acc.bufferView];
  const tinygltf::Buffer& buf = model.buffers[bv.buffer];

  const float* data =
      reinterpret_cast<const float*>(&buf.data[bv.byteOffset + acc.byteOffset]);

  for (size_t i = 0; i < skin.joints.size(); ++i) {
    skin.joints[i].node = gltfSkin.joints[i];
    skin.joints[i].inverseBind = JPH::Mat44::sLoadFloat4x4(
      reinterpret_cast<const JPH::Float4*>(data + i * 16));
  }
  skin.animations = LoadAnimations(model);
  return skin;
}

const float* GetFloatData(const tinygltf::Model& model,
                          const tinygltf::Accessor& acc) {
  assert(acc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
  const auto& view = model.bufferViews[acc.bufferView];
  const auto& buffer = model.buffers[view.buffer];
  return reinterpret_cast<const float*>(buffer.data.data() + view.byteOffset +
                                        acc.byteOffset);
  // (view.byteStride != 0) in gltf spec?
}

std::vector<Scene::Animation> ModelLoader::LoadAnimations(
  const tinygltf::Model& model) {
  std::vector<Scene::Animation> animations(model.animations.size());
  for (int i = 0; i < model.animations.size(); ++i) {
    const auto& animation = model.animations[i];
    animations[i].name = animation.name;
    animations[i].channels = std::vector<Scene::AnimationChannel>(animation.channels.size());
    for (int j = 0; j < animation.channels.size(); ++j) {
      const auto& channel = animation.channels[j];
      animations[i].channels[j].sampler = channel.sampler;
      animations[i].channels[j].target_node = channel.target_node;
      animations[i].channels[j].target_path = channel.target_path;
    }
    animations[i].samplers = std::vector<Scene::AnimationSampler>(animation.samplers.size());
    for (int j = 0; j < model.animations[i].samplers.size(); ++j) {
      const auto& sampler = model.animations[i].samplers[j];
      animations[i].samplers[j].interpolation = sampler.interpolation;
      const tinygltf::Accessor& in_acc = model.accessors[sampler.input];
      const tinygltf::Accessor& out_acc = model.accessors[sampler.output];
      const float* times = GetFloatData(model, in_acc);
      const float* values = GetFloatData(model, out_acc);
      animations[i].samplers[j].times = std::vector<float>(times, times + in_acc.count);
      int num_components = tinygltf::GetNumComponentsInType(out_acc.type);
      size_t total_floats = out_acc.count * num_components;
      animations[i].samplers[j].values = std::vector<float>(values, values + total_floats);
    }
  }
  return animations;
}

tinygltf::Model LoadModel(tinygltf::TinyGLTF& loader, std::string_view path) {
  std::string err, warn;
  tinygltf::Model model;
  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, path.data());
  if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
  if (!err.empty()) std::cout << "ERR: " << err << std::endl;
  if (!res) throw "Failed to load glTF";
  return model;
}

void UploadBuffers(GLuint* vao, GLuint* vbo, GLuint* ebo,
  ModelLoader::BufferData& buffer_data) {
  glGenVertexArrays(1, vao);
  glBindVertexArray(*vao);
  glGenBuffers(1, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, *vbo);

  size_t pos_size = buffer_data.all_positions.size();
  size_t norm_size = buffer_data.all_normals.size();
  size_t uv_size = buffer_data.all_uvs.size();
  size_t tangent_size = buffer_data.all_tangents.size();

  // 1. Calculate proper byte offsets sequentially
  size_t pos_offset = 0;
  size_t norm_offset = pos_offset + pos_size;
  size_t uv_offset = norm_offset + norm_size;
  size_t tangent_offset = uv_offset + uv_size;

  size_t total_vbo_size = tangent_offset + tangent_size;

  glBufferData(GL_ARRAY_BUFFER, total_vbo_size, nullptr, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, pos_offset, pos_size,
                  buffer_data.all_positions.data());
  glBufferSubData(GL_ARRAY_BUFFER, norm_offset, norm_size,
                  buffer_data.all_normals.data());
  glBufferSubData(GL_ARRAY_BUFFER, uv_offset, uv_size, buffer_data.all_uvs.data());
  glBufferSubData(GL_ARRAY_BUFFER, tangent_offset, tangent_size,
                  buffer_data.all_tangents.data());

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)pos_offset);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)norm_offset);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)uv_offset);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)tangent_offset);

  glGenBuffers(1, ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer_data.all_indices.size(),
               buffer_data.all_indices.data(), GL_STATIC_DRAW);

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);

  glBindVertexArray(0);
}

void UploadBuffers(GLuint* vao, GLuint* vbo, GLuint* ebo,
  ModelLoader::BufferData& buffer_data,
  ModelLoader::BufferDataAnimated& buffer_data_animated) {
  glGenVertexArrays(1, vao);
  glBindVertexArray(*vao);
  glGenBuffers(1, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, *vbo);

  size_t pos_size = buffer_data.all_positions.size();
  size_t norm_size = buffer_data.all_normals.size();
  size_t uv_size = buffer_data.all_uvs.size();
  size_t tangent_size = buffer_data.all_tangents.size();
  size_t joint_size = buffer_data_animated.all_joints.size();
  size_t weight_size = buffer_data_animated.all_weights.size();

  size_t pos_offset = 0;
  size_t norm_offset = pos_size;
  size_t uv_offset = norm_offset + norm_size;
  size_t tangent_offset = uv_offset + uv_size;
  size_t joint_offset = tangent_offset + tangent_size;
  size_t weight_offset = joint_offset + joint_size;

  size_t total_vbo_size = weight_offset + weight_size;

  glBufferData(GL_ARRAY_BUFFER, total_vbo_size, nullptr, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, pos_offset, pos_size,
                  buffer_data.all_positions.data());
  glBufferSubData(GL_ARRAY_BUFFER, norm_offset, norm_size,
                  buffer_data.all_normals.data());
  glBufferSubData(GL_ARRAY_BUFFER, uv_offset, uv_size,
                  buffer_data.all_uvs.data());
  glBufferSubData(GL_ARRAY_BUFFER, tangent_offset, tangent_size,
                  buffer_data.all_tangents.data());
  glBufferSubData(GL_ARRAY_BUFFER, joint_offset, joint_size,
                  buffer_data_animated.all_joints.data());
  glBufferSubData(GL_ARRAY_BUFFER, weight_offset, weight_size,
                  buffer_data_animated.all_weights.data());

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)pos_offset);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)norm_offset);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)uv_offset);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)tangent_offset);
  glEnableVertexAttribArray(4);
  glVertexAttribIPointer(4, 4, buffer_data_animated.joints_type, 0, (void*)joint_offset);
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, (void*)weight_offset);

  glGenBuffers(1, ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer_data.all_indices.size(),
               buffer_data.all_indices.data(), GL_STATIC_DRAW);

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);
  glVertexAttribDivisor(4, 0);
  glVertexAttribDivisor(5, 0);

  glBindVertexArray(0);
}

SceneNode* CreateObjectNode(const tinygltf::Model& model, int root_node_id,
  const std::vector<Scene::Mesh>& meshes) {
  std::function<SceneNode*(int)> dfs =
    [&](int id) {
      auto node = new SceneNode();
      const auto& scene_node = model.nodes[id];
      node->mesh_index = scene_node.mesh;
      node->local_transform.r = GetNodeRotation(model.nodes[id]);
      node->local_transform.t = GetNodeTranslation(model.nodes[id]);
      node->local_transform.s = GetNodeScale(model.nodes[id]);
      node->node_id = id;
      if (scene_node.mesh != -1) {
        const auto& mesh = meshes[scene_node.mesh];
        node->local_bounds = JPH::AABox(JPH::Vec3(mesh.min.x, mesh.min.y, mesh.min.z),
        JPH::Vec3(mesh.max.x, mesh.max.y, mesh.max.z));
      }
      for (auto child_id : scene_node.children) {
        node->children.push_back(dfs(child_id));
      }
      return node;
    };
  return dfs(root_node_id );
}

void ParseZone(const tinygltf::Model& model,
const tinygltf::Node& tile_node, Scene::Zone* zone,
const std::vector<Scene::Mesh>& meshes) {
  for (const auto& child_id : tile_node.children) {
    zone->object_nodes.push_back(CreateObjectNode(model, child_id, meshes));
  }
}

void ParseCharacter(const tinygltf::Model& model,
    int node_id, Scene::Tile* tile,
    const std::vector<Scene::Mesh>& meshes) {
  const auto& node = model.nodes[node_id];
  tile->characters.push_back(CreateObjectNode(model, node_id, meshes));
  // patrol path is linear thing, so directed line
  // std::vector<JPH::Vec3> patrol_path;
  // patrol_path.push_back(ReadVec3(node.translation, 0));
  // auto dfs = [&](auto& self, const tinygltf::Node& node) -> void {
  //   if (!node.children.empty()) {
  //     const auto& child_node = model.nodes[node.children[0]];
  //     if (!node.translation.empty()) {
  //       patrol_path.push_back(ReadVec3(child_node.translation, 0));
  //     }
  //     self(self, child_node);
  //   }
  // };
  // dfs(dfs, node);
}

void Scene::ConnectZonesWithPortals(int tile_id) {
  std::vector<Scene::Zone*>& zones = scene_data_.tiles[0]->zones;
  std::vector<Scene::Portal>& portals = scene_data_.tiles[0]->portals;

  for (size_t p = 0; p < portals.size(); ++p) {
    Scene::Portal& portal_data = portals[p];

    // Safety check: ensure the portal node actually exists
    if (!portal_data.portal) continue;

    // 1. Expand the portal's AABB by a tiny epsilon.
    // Because portals sit perfectly flush on the 2D boundary of two rooms,
    // floating-point precision can cause strict AABB overlaps to fail.
    JPH::Vec3 epsilon = JPH::Vec3::sReplicate(0.05f);
    JPH::AABox expanded_portal_box;
    expanded_portal_box.mMin = portal_data.portal->global_bounds.mMin- epsilon;
    expanded_portal_box.mMin.SetY(std::numeric_limits<float>::lowest());
    expanded_portal_box.mMax = portal_data.portal->global_bounds.mMax + epsilon;
    expanded_portal_box.mMax.SetY(std::numeric_limits<float>::max());

    int connected_count = 0;

    // 2. Test this portal against all zones
    for (size_t z = 0; z < zones.size(); ++z) {
      Scene::Zone* zone = zones[z];

      if (zone->bounds.Overlaps(expanded_portal_box)) {

        // 3. Link the Portal to the Zone
        if (connected_count == 0) {
          portal_data.connected_zone_index_1 = z;
        } else if (connected_count == 1) {
          portal_data.connected_zone_index_2 = z;
        }

        // 4. Link the Zone back to the Portal
        // (This stores a pointer to the element in the portals vector)
        zone->portals.push_back(&portal_data);

        connected_count++;

        // 5. Early exit: A portal can only connect two zones
        if (connected_count == 2) {
          break;
        }
      }
    }

    // 6. Validation Logging
    if (connected_count < 2) {
      std::cerr << "Warning: Portal Node ID " << portal_data.portal->node_id
                << " connected to only " << connected_count << " zones.\n";
    }
  }
}

//TODO: leak new

void ParseTile(const tinygltf::Model& model,
  const tinygltf::Node& tile_node, Scene::Tile* tile,
  const std::vector<Scene::Mesh>& meshes) {
  for (const auto& child_id : tile_node.children) {
    const auto& node = model.nodes[child_id];
    if (node.mesh == -1) {
      continue;
    }
    Scene::Type type = meshes[node.mesh].type;
    if (type == Scene::Type::Zone) {
      auto zone = new Scene::Zone();
      const auto& mesh = meshes[node.mesh];
      zone->bounds = JPH::AABox(JPH::Vec3(mesh.min.x, mesh.min.y, mesh.min.z),
        JPH::Vec3(mesh.max.x, mesh.max.y, mesh.max.z));
      tile->zones.push_back(zone);
      ParseZone(model, node, zone, meshes);
    } else if (type == Scene::Type::Character) {
      ParseCharacter(model, child_id, tile, meshes);
    } else if (type == Scene::Type::Portal) {
      //tile->portals.push_back(CreateObjectNode(model, child_id, meshes));
    } else {
      Scene::Portal portal;
      if (!node.translation.empty()) {
        portal.position = ReadVec3(node.translation, 0);
      }
      if (!node.rotation.empty()) {
        portal.rotation = ReadQuat(node.rotation, 0);
      }
      for (auto grand_child_id : node.children) {
        const auto& child_node = model.nodes[grand_child_id];
        Scene::Type child_type = meshes[child_node.mesh].type;
        if (child_type == Scene::Type::None) {
          portal.render = CreateObjectNode(model, grand_child_id, meshes);
        } else if (child_type == Scene::Type::Portal) {
          portal.portal = CreateObjectNode(model, grand_child_id, meshes);
        } else {
          std::cerr << "wrong obj type for the tile children" << std::endl;
        }
      }
      tile->portals.push_back(portal);
    }
  }
  //ConnectZonesWithPortals(tile->zones, tile->portals);
}

void ReadSceneHierarchy(
  Scene::SceneData& scene,
  const tinygltf::Model& model) {
  const std::vector<Scene::Mesh>& meshes = scene.meshes;
  for (size_t i = 0; i < model.nodes.size(); ++i) {
    const auto& node = model.nodes[i];
    auto mesh_id = node.mesh;
    if (mesh_id == -1) {
      continue;
    }
    Scene::Type type = meshes[mesh_id].type;
    if (type == Scene::Type::Tile) {
      auto tile = new Scene::Tile();

      const auto& mesh = meshes[node.mesh];
      tile->bounds = JPH::AABox(JPH::Vec3(mesh.min.x, mesh.min.y, mesh.min.z),
        JPH::Vec3(mesh.max.x, mesh.max.y, mesh.max.z));

      scene.tiles.push_back(tile);
      ParseTile(model, node, tile, meshes);
    } else if (type == Scene::Type::Player) {
      scene.player_node = CreateObjectNode(model, i, meshes);
    }
  }
  // for (size_t i = 0; i < model.nodes.size(); ++i) {
  //   const tinygltf::Node& gltfNode = model.nodes[i];
  //
  //   for (int childIndex : gltfNode.children) {
  //     nodes[i]->children.push_back(nodes[childIndex]);
  //     nodes[childIndex]->parent = nodes[i];
  //   }
  // }
}

void ReadNodeHierarchy(
  std::vector<SceneNode*>& nodes,
  const tinygltf::Model& model,
  const std::vector<Scene::Mesh>& meshes) {
  nodes.clear();
  nodes.resize(model.nodes.size());
  for (size_t i = 0; i < model.nodes.size(); ++i) {
    nodes[i] = CreateObjectNode(model, i, meshes);
  }
  for (size_t i = 0; i < model.nodes.size(); ++i) {
    const tinygltf::Node& gltfNode = model.nodes[i];

    for (int childIndex : gltfNode.children) {
      //nodes[i]->children.push_back(nodes[childIndex]);
      nodes[childIndex]->parent = nodes[i];
    }
  }
}

void ModelLoader::LoadDebugShapes(std::string_view path) {
  scene_.scene_data_ = Scene::SceneData(); //TODO: separate func?

  auto model = LoadModel(loader_, path);
  auto buffer_data = LoadBuffers(
  model, scene_.scene_dbg_shapes_.meshes);
  ReadSceneHierarchy(scene_.scene_dbg_shapes_, model);
  UploadBuffers(&scene_.scene_dbg_shapes_.vao,
    &scene_.scene_dbg_shapes_.vbo,
    &scene_.scene_dbg_shapes_.ebo,
    buffer_data);
}

void ModelLoader::LoadScene(std::string_view path) {
  auto model = LoadModel(loader_, path);
  auto buffer_data = LoadBuffers(
    model, scene_.scene_data_.meshes);
  ReadSceneHierarchy(scene_.scene_data_, model);
  stbi_set_flip_vertically_on_load(false);
  scene_.materials = LoadMaterials(path, model);
  stbi_set_flip_vertically_on_load(true);
  UploadBuffers(&scene_.scene_data_.vao,
    &scene_.scene_data_.vbo,
    &scene_.scene_data_.ebo,
    buffer_data);
}

void MarkLowerBodyNodes(SceneNode* node, const std::vector<SceneNode*>& nodes, std::vector<bool>& is_lower_body) {
  is_lower_body[node->node_id] = true;
  for (auto child : node->children) {
    MarkLowerBodyNodes(child, nodes, is_lower_body);
  }
}

void BuildLowerBodyMask(std::vector<bool>& is_lower_body, int pelvis_node_id, const std::vector<SceneNode*>& nodes) {
  if (pelvis_node_id >= 0 && pelvis_node_id < static_cast<int>(nodes.size())) {
    MarkLowerBodyNodes(nodes[pelvis_node_id], nodes, is_lower_body);
  }
}

void ModelLoader::LoadCharacters(std::string_view skeleton_path,
    std::vector<std::string_view> skin_paths) {
  for (const auto& path : skin_paths) {
    Scene::CharacterData character;
    auto model = LoadModel(loader_, path);
    auto buffer_data = LoadBuffers(
    model, character.meshes);
    ReadNodeHierarchy(character.nodes, model, character.meshes);
    auto buffer_data_animated = LoadBuffersAnimated(model);

    stbi_set_flip_vertically_on_load(false);
    character.material = LoadMaterial(path, model, model.materials[0]);
    stbi_set_flip_vertically_on_load(true);

    UploadBuffers(&character.vao, &character.vbo, &character.ebo,
      buffer_data, buffer_data_animated);
    scene_.character_skins_.push_back(std::move(character));
  }

  auto skeleton_model = LoadModel(loader_, skeleton_path);
  scene_.character_rig_.core_rig = LoadCoreRig(skeleton_model);
  scene_.character_rig_.mapping = CreateCharacterAnimationMapping(
    scene_.character_rig_.core_rig.animations);
  LoadDeltas(
    scene_.character_rig_.mapping,
    scene_.character_rig_.core_rig.animations,
    scene_.character_skins_[0].nodes,
    scene_.character_rig_.default_deltas,
    scene_.character_rig_.pistol_deltas);

  //int hips_id = 0;
  //int spine_id = 0;
  for (int i = 0; i < skeleton_model.nodes.size(); ++i) {
    const auto& n = skeleton_model.nodes[i];
    // mixamorig6:RightUpLeg"
    if (n.name == "mixamorig6:Camera") {
      scene_.character_rig_.head_bone_id = i;
      std::cout << "camera is " << i << std::endl;
    } else if (n.name == "mixamorig6:Weapon") {
      scene_.character_rig_.hand_bone_id = i;
      std::cout << "weapon is " << i << std::endl;
    } else if (n.name == "mixamorig6:Hips") {
      scene_.character_rig_.hips_id = i;
      std::cout << "hips is " << i << std::endl;
    } else if (n.name == "mixamorig6:Spine") {
      scene_.character_rig_.spine0_id = i;
      std::cout << "spine0 is " << i << std::endl;
    } else if (n.name == "mixamorig6:Spine1") {
      scene_.character_rig_.spine1_id = i;
      std::cout << "spine1 is " << i << std::endl;
    } else if (n.name == "mixamorig6:Spine2") {
      scene_.character_rig_.spine2_id = i;
      std::cout << "spine2 is " << i << std::endl;
    }
  }
  //auto& nodes = scene_.character_skins_[0].nodes;
  //scene_.character_rig_.is_lower_body_mask = std::vector<bool>(nodes.size(), false);
  //BuildLowerBodyMask(scene_.character_rig_.is_lower_body_mask, left_shoulder_id, nodes);
  //BuildLowerBodyMask(scene_.character_rig_.is_lower_body_mask, right_shoulder_id, nodes);
}

void ModelLoader::LoadWeapon(std::vector<std::string_view> paths) {
  for (const auto& path : paths) {
    Scene::WeaponData weapon;
    auto model = LoadModel(loader_, path);
    auto buffer_data = LoadBuffers(
    model, weapon.meshes);
    ReadNodeHierarchy(weapon.nodes, model, weapon.meshes);
    auto buffer_data_animated = LoadBuffersAnimated(model);

    stbi_set_flip_vertically_on_load(false);
    weapon.material = LoadMaterial(path, model, model.materials[0]);
    stbi_set_flip_vertically_on_load(true);

    UploadBuffers(&weapon.vao, &weapon.vbo, &weapon.ebo,
      buffer_data, buffer_data_animated);
    weapon.rig.core_rig = LoadCoreRig(model);
    weapon.rig.mapping = CreateWeaponAnimationMapping(weapon.rig.core_rig.animations);
    scene_.weapons_.push_back(std::move(weapon));
  }
}

Scene::Type GetModelType(const tinygltf::Mesh& mesh) {
  auto type = Scene::Type::Static;
  if (mesh.extras.Has("body")) {
    std::string collision_value = mesh.extras.Get("body").Get<std::string>();
    if (collision_value == "dynamic") {
      type = Scene::Type::Dynamic;
    } else if (collision_value == "light") {
      type = Scene::Type::PointLight;
    } else if (collision_value == "hinge") {
      type = Scene::Type::Hinge;
    } else if (collision_value == "door") {
      type = Scene::Type::Door;
    } else if (collision_value == "portal") {
      type = Scene::Type::Portal;
    } else if (collision_value == "zone") {
      type = Scene::Type::Zone;
    } else if (collision_value.starts_with("tile")) {
      type = Scene::Type::Tile;
    } else if (collision_value == "terrain") {
      type = Scene::Type::Terrain;
    } else if (collision_value == "character") {
      type = Scene::Type::Character;
    } else if (collision_value == "player") {
      type = Scene::Type::Player;
    } else if (collision_value == "none") {
      type = Scene::Type::None;
    }
  }
  return type;
}

Scene::CollisionType GetCollisionType(const tinygltf::Mesh& mesh) {
  auto type = Scene::CollisionType::Cube;
  if (mesh.extras.Has("collision")) {
    std::string collision_value =
        mesh.extras.Get("collision").Get<std::string>();
    if (collision_value == "sphere") {
      type = Scene::CollisionType::Sphere;
    } else if (collision_value == "capsule") {
      type = Scene::CollisionType::Capsule;
    } else if (collision_value == "cylinder") {
      type = Scene::CollisionType::Cylinder;
    } else if (collision_value == "cube") {
      type = Scene::CollisionType::Cube;
    }
  }
  return type;
}

const tinygltf::Accessor& LoadIndexBuffer(const tinygltf::Model& model,
                                          const tinygltf::Primitive& primitive,
                                          std::vector<std::uint8_t>& data) {
  const auto& idxAccessor = model.accessors.at(primitive.indices);
  const auto& idxView = model.bufferViews.at(idxAccessor.bufferView);
  const auto& idxBuffer = model.buffers.at(idxView.buffer);
  size_t idxElementSize =
      (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) ? 4
                                                                          : 2;
  size_t padding =
      (idxElementSize - (data.size() % idxElementSize)) % idxElementSize;
  data.insert(data.end(), padding,
              0);  // Apply padding bytes

  size_t idxBytes = idxAccessor.count * idxElementSize;
  const uint8_t* idxStart =
      &idxBuffer.data.at(0) + idxView.byteOffset + idxAccessor.byteOffset;

  data.insert(data.end(), idxStart, idxStart + idxBytes);
  return idxAccessor;
}

const tinygltf::Accessor& LoadBufferSafely(
    std::string_view name, const tinygltf::Model& model,
    const tinygltf::Primitive& primitive, std::vector<std::uint8_t>& out_data) {
  // 1. Find the accessor and buffer data
  const auto& accessor =
      model.accessors.at(primitive.attributes.at(name.data()));
  const auto& bufferView = model.bufferViews.at(accessor.bufferView);
  const auto& buffer = model.buffers.at(bufferView.buffer);

  // 2. Calculate pointers and sizes
  const uint8_t* startPtr =
      &buffer.data[bufferView.byteOffset + accessor.byteOffset];

  // How big is ONE element? (e.g., vec4 of floats = 16 bytes)
  size_t componentSize =
      tinygltf::GetComponentSizeInBytes(accessor.componentType);
  size_t elementCount = tinygltf::GetNumComponentsInType(accessor.type);
  size_t actualElementByteSize = componentSize * elementCount;

  // How many bytes to jump to the next vertex?
  size_t stride = accessor.ByteStride(bufferView);

  // 3. Extract the data element by element, skipping the interleaved gaps
  for (size_t i = 0; i < accessor.count; ++i) {
    const uint8_t* currentElementPtr = startPtr + (i * stride);

    out_data.insert(out_data.end(), currentElementPtr,
                    currentElementPtr + actualElementByteSize);
  }

  return accessor;
}

void LoadRiggedBuffers(
    const tinygltf::Model& model,
    const tinygltf::Primitive& primitive,
    ModelLoader::BufferDataAnimated& data) {
  LoadBufferSafely("WEIGHTS_0", model, primitive, data.all_weights);
  auto jointsAccessor =
      LoadBufferSafely("JOINTS_0", model, primitive, data.all_joints);
  if (jointsAccessor.componentType ==
      TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
    data.joints_type = GL_UNSIGNED_SHORT;
      } else if (jointsAccessor.componentType ==
                 TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
        data.joints_type = GL_UNSIGNED_BYTE;
                 } else {
                   std::cerr << "WARNING: Unexpected joint component type!\n";
                 }
}

//TODO: mesh != primitive
//TODO: render-nodes vector

ModelLoader::BufferDataAnimated ModelLoader::LoadBuffersAnimated(
  const tinygltf::Model& model) {
  BufferDataAnimated data;
  for (const auto& mesh : model.meshes) {
    for (const auto& primitive : mesh.primitives) {
      LoadRiggedBuffers(model, primitive, data);
    }
  }
  return data;
}
ModelLoader::BufferData ModelLoader::LoadBuffers(const tinygltf::Model& model,
  std::vector<Scene::Mesh>& meshes) {
  BufferData data;
  //TODO: for some meshes we don't need the geometry (like zones/characters)
  //TODO: are these extras of the node OR of the mesh?
  std::vector<int> tile_ids;
  for (const auto& mesh : model.meshes) {
    Scene::Mesh new_mesh;
    new_mesh.collision_type = GetCollisionType(mesh);
    new_mesh.type = GetModelType(mesh);
    glm::vec3 mesh_min(std::numeric_limits<float>::max());
    glm::vec3 mesh_max(std::numeric_limits<float>::lowest());
    for (const auto& primitive : mesh.primitives) {
      const auto& posAccessor =
          LoadBufferSafely("POSITION", model, primitive, data.all_positions);
      LoadBufferSafely("NORMAL", model, primitive, data.all_normals);
      LoadBufferSafely("TEXCOORD_0", model, primitive, data.all_uvs);
      LoadBufferSafely("TANGENT", model, primitive, data.all_tangents);
      glm::vec3 min(posAccessor.minValues[0], posAccessor.minValues[1],
                    posAccessor.minValues[2]);
      glm::vec3 max(posAccessor.maxValues[0], posAccessor.maxValues[1],
                    posAccessor.maxValues[2]);
      mesh_min = glm::min(mesh_min, min);
      mesh_max = glm::max(mesh_max, max);

      size_t index_byte_offset = data.all_indices.size();
      const auto& idxAccessor =
          LoadIndexBuffer(model, primitive, data.all_indices);
      GLenum gl_idx_type =
          (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
              ? GL_UNSIGNED_INT
              : GL_UNSIGNED_SHORT;

      int material_id = primitive.material;
      if (material_id == -1) {
        material_id = 0;
      }
      material_id = 0;
      Scene::Primitive new_primitive(idxAccessor.count, index_byte_offset,
                        data.current_base_vertex, gl_idx_type,
                        static_cast<uint32_t>(material_id));
      new_mesh.primitives.push_back(new_primitive);
      data.current_base_vertex += posAccessor.count;
    }
    new_mesh.min = mesh_min;
    new_mesh.max = mesh_max;
    meshes.push_back(new_mesh);
  }
  return data;
}

Material ModelLoader::LoadMaterial(std::string_view path,
                                   tinygltf::Model& model,
                                   const tinygltf::Material& m) {
  Material material;

  auto albedo_tex_id = m.pbrMetallicRoughness.baseColorTexture.index;
  material.albedo = LoadTexture(path, model, albedo_tex_id);

  auto normal_tex_id = m.normalTexture.index;
  material.normal = LoadTexture(path, model, normal_tex_id);

  auto rough_metal_ao_tex_id = m.pbrMetallicRoughness.metallicRoughnessTexture.index;
  material.rough_metal_ao = LoadTexture(path, model, rough_metal_ao_tex_id);

  return material;
}

GLuint CreateTextureArray(int width, int height, int layers,
                          GLenum internalFormat) {
  GLuint tex;
  glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tex);
  glTextureStorage3D(tex, 1, internalFormat, width, height, layers);
  glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);
  return tex;
}

MaterialArray ModelLoader::LoadMaterials(
    std::string_view path, tinygltf::Model& model) {
  MaterialArray pack;
  pack.count = static_cast<uint32_t>(model.materials.size());
  pack.count = 1;
  pack.albedo =
      CreateTextureArray(materialWidth, materialHeight, pack.count, GL_RGBA8);
  pack.normal =
      CreateTextureArray(materialWidth, materialHeight, pack.count, GL_RGBA8);
  pack.rough_metal_ao =
      CreateTextureArray(materialWidth, materialHeight, pack.count, GL_RGBA8);
  for (int layer = 0; layer < pack.count; ++layer) {
    const auto& m = model.materials[layer];
    auto albedo = LoadTextureRaw(path, model, m.pbrMetallicRoughness.baseColorTexture.index);
    auto normal = LoadTextureRaw(path, model, m.normalTexture.index);
    auto ao_rough_metal = LoadTextureRaw(path, model, m.pbrMetallicRoughness.metallicRoughnessTexture.index);
    glTextureSubImage3D(
      pack.albedo, 0, 0, 0, layer, materialWidth, materialHeight, 1, GL_RGBA,
      GL_UNSIGNED_BYTE, albedo.data());
    glTextureSubImage3D(
      pack.normal, 0, 0, 0, layer, materialWidth, materialHeight, 1, GL_RGBA,
      GL_UNSIGNED_BYTE, normal.data());
    glTextureSubImage3D(
      pack.rough_metal_ao, 0, 0, 0, layer, materialWidth, materialHeight, 1, GL_RGBA,
      GL_UNSIGNED_BYTE, ao_rough_metal.data());
  }
  glGenerateTextureMipmap(pack.albedo);
  glGenerateTextureMipmap(pack.normal);
  glGenerateTextureMipmap(pack.rough_metal_ao);
  return pack;
}

Texture ModelLoader::LoadTexture(std::string_view path,
                                 const tinygltf::Model& model, int tex_id) {
  if (tex_id == -1) {
    throw "model textures load failed 1";
  }
  auto& texture = model.textures[tex_id];
  auto image_index = texture.source;
  if (image_index == -1) {
    throw "model textures load failed";
  }
  namespace fs = std::filesystem;
  fs::path tex_path{path};
  tex_path = tex_path.parent_path();
  fs::path image_uri{model.images[image_index].uri};
  tex_path /= image_uri.make_preferred();
  return Texture(tex_path.string(), Texture::Type::TerrainRGBA8);
}

std::vector<uint8_t> ModelLoader::LoadTextureRaw(
    std::string_view path, const tinygltf::Model& model, int tex_id) {
  if (tex_id == -1) {
    throw "model textures load failed 1";
  }
  auto& texture = model.textures[tex_id];
  auto image_index = texture.source;
  if (image_index == -1) {
    throw "model textures load failed";
  }
  namespace fs = std::filesystem;
  fs::path tex_path{path};
  tex_path = tex_path.parent_path();
  fs::path image_uri{model.images[image_index].uri};
  tex_path /= image_uri.make_preferred();

  int width, height;
  GLint channels;
  GLint desired_channels = 4;
  std::vector<uint8_t> data(materialWidth * materialHeight * desired_channels);
  unsigned char* data_uc =
    stbi_load(tex_path.string().data(), &width, &height, &channels, desired_channels);
  if (!data_uc) {
    std::cerr << "failed to load texture " << tex_path << std::endl;
  }
  std::memcpy(data.data(), data_uc, data.size());
  stbi_image_free(data_uc);
  if ((width & 3) || (height & 3)) {
    std::cerr << "need GL_UNPACK_ALIGNMENT for size " << width << ' '
              << height << std::endl;
  }
  return data;
}
