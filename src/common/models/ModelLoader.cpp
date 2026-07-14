#include "ModelLoader.h"

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

Scene::Skin ModelLoader::LoadSkin(const tinygltf::Model& model) {
  const tinygltf::Skin& gltfSkin = model.skins[0];
  Scene::Skin skin;
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
      // Time is always a SCALAR, so in_acc.count is safe.
      animations[i].samplers[j].times = std::vector<float>(times, times + in_acc.count);
      // FIX: Multiply the count by the number of components (3 for VEC3, 4 for VEC4)
      int num_components = tinygltf::GetNumComponentsInType(out_acc.type);
      size_t total_floats = out_acc.count * num_components;
      animations[i].samplers[j].values = std::vector<float>(values, values + total_floats);
      // animations[i].samplers[j].times = std::vector(times, times + in_acc.count);
      // animations[i].samplers[j].values = std::vector(values, values + out_acc.count);
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

Scene::ModelNode* CreateObjectNode(const tinygltf::Model& model, int root_node_id) {
  std::function<Scene::ModelNode*(int)> dfs =
    [&](int id) {
      auto node = new Scene::ModelNode();
      const auto& scene_node = model.nodes[id];
      node->mesh_index = scene_node.mesh;
      node->local_transform.r = GetNodeRotation(model.nodes[id]);
      node->local_transform.t = GetNodeTranslation(model.nodes[id]);
      node->local_transform.s = GetNodeScale(model.nodes[id]);
      node->node_id = id;
      for (auto child_id : scene_node.children) {
        node->children.push_back(dfs(child_id));
      }
      return node;
    };
  return dfs(root_node_id );
}

void ParseZone(const tinygltf::Model& model,
  const tinygltf::Node& tile_node, Scene::Zone* zone) {
  for (const auto& child_id : tile_node.children) {
    zone->object_nodes.push_back(CreateObjectNode(model, child_id));
  }
}

//TODO: leak new

void ParseTile(const tinygltf::Model& model,
  const tinygltf::Node& tile_node, Scene::Tile* tile,
  const std::vector<Scene::Mesh>& meshes) {
  for (const auto& child_id : tile_node.children) {
    const auto& node = model.nodes[child_id];
    Scene::Type type = meshes[node.mesh].type;
    if (type == Scene::Type::Zone) {
      auto zone = new Scene::Zone();
      tile->zones.push_back(zone);
      ParseZone(model, node, zone);
    } else {
      tile->object_nodes.push_back(CreateObjectNode(model, child_id));
    }
  }
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
      scene.tiles.push_back(tile);
      ParseTile(model, node, tile, meshes);
    } else if (type == Scene::Type::Player) {
      scene.player_node = CreateObjectNode(model, i);
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
  std::vector<Scene::ModelNode*>& nodes,
  const tinygltf::Model& model) {
  nodes.clear();
  nodes.resize(model.nodes.size());
  for (size_t i = 0; i < model.nodes.size(); ++i) {
    nodes[i] = CreateObjectNode(model, i);
  }
  for (size_t i = 0; i < model.nodes.size(); ++i) {
    const tinygltf::Node& gltfNode = model.nodes[i];

    for (int childIndex : gltfNode.children) {
      nodes[i]->children.push_back(nodes[childIndex]);
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

void ModelLoader::LoadCharacters(std::string_view skeleton_path,
    std::vector<std::string_view> skin_paths) {
  for (const auto& path : skin_paths) {
    Scene::CharacterData character;
    auto model = LoadModel(loader_, path);
    auto buffer_data = LoadBuffers(
    model, character.meshes);
    ReadNodeHierarchy(character.nodes, model);
    auto buffer_data_animated = LoadBuffersAnimated(model);

    stbi_set_flip_vertically_on_load(false);
    character.material = LoadMaterial(path, model, model.materials[0]);
    stbi_set_flip_vertically_on_load(true);

    UploadBuffers(&character.vao, &character.vbo, &character.ebo,
      buffer_data, buffer_data_animated);
    scene_.character_skins_.push_back(std::move(character));
  }

  auto skeleton_model = LoadModel(loader_, skeleton_path);
  scene_.character_rig_ = LoadSkin(skeleton_model);
  for (int i = 0; i < skeleton_model.nodes.size(); ++i) {
    const auto& n = skeleton_model.nodes[i];
    if (n.name == "mixamorig6:HeadTop_End") {
      scene_.character_rig_.head_bone_id = i;
      std::cout << "head is " << i << std::endl;
    } else if (n.name == "mixamorig6:RightHand") {
      scene_.character_rig_.hand_bone_id = i;
      std::cout << "hand is " << i << std::endl;
    }
  }
}

void ModelLoader::LoadWeapon(std::vector<std::string_view> paths) {
  for (const auto& path : paths) {
    Scene::WeaponData weapon;
    auto model = LoadModel(loader_, path);
    auto buffer_data = LoadBuffers(
    model, weapon.meshes);
    ReadNodeHierarchy(weapon.nodes, model);
    auto buffer_data_animated = LoadBuffersAnimated(model);

    stbi_set_flip_vertically_on_load(false);
    weapon.material = LoadMaterial(path, model, model.materials[0]);
    stbi_set_flip_vertically_on_load(true);

    UploadBuffers(&weapon.vao, &weapon.vbo, &weapon.ebo,
      buffer_data, buffer_data_animated);
    weapon.rig = LoadSkin(model);
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
    } else if (collision_value == "zone") {
      type = Scene::Type::Zone;
    } else if (collision_value == "tile") {
      type = Scene::Type::Tile;
    } else if (collision_value == "terrain") {
      type = Scene::Type::Terrain;
    } else if (collision_value == "character") {
      type = Scene::Type::Character;
    } else if (collision_value == "player") {
      type = Scene::Type::Player;
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
    auto collision_type = GetCollisionType(mesh);
    auto mesh_type = GetModelType(mesh);
    glm::vec3 mesh_min(std::numeric_limits<float>::max());
    glm::vec3 mesh_max(std::numeric_limits<float>::min());
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
      meshes.push_back({idxAccessor.count, index_byte_offset,
                        data.current_base_vertex, gl_idx_type, min, max,
                        static_cast<uint32_t>(material_id), mesh_type, collision_type});
      data.current_base_vertex += posAccessor.count;
      break; //TODO: now we keep only one primitive per mesh
    }
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
