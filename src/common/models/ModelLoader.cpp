#include "ModelLoader.h"

#include <filesystem>
#include <iostream>

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Animator.h"

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

Scene* ModelLoader::LoadScene(std::string_view collisions_path,
                              std::string_view characters_path,
                              std::string_view scene_path) {
  scene_ = std::make_unique<Scene>();
  {
    BufferData data;
    LoadBufferMerge(collisions_path, scene_->models, scene_->meshes, data, false);

    {
      auto scene_model =
       LoadBufferMerge(scene_path, scene_->models, scene_->meshes, data, false);
      stbi_set_flip_vertically_on_load(false);
      LoadMaterials(scene_path, scene_model, scene_->materials);
      stbi_set_flip_vertically_on_load(true);
    }

    glGenVertexArrays(1, &scene_->vao);
    glBindVertexArray(scene_->vao);
    glGenBuffers(1, &scene_->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, scene_->vbo);

    size_t pos_size = data.all_positions.size();
    size_t norm_size = data.all_normals.size();
    size_t uv_size = data.all_uvs.size();
    size_t total_vbo_size = pos_size + norm_size + uv_size;

    glBufferData(GL_ARRAY_BUFFER, total_vbo_size, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, pos_size, data.all_positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, pos_size, norm_size,
                    data.all_normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, pos_size + norm_size, uv_size,
                    data.all_uvs.data());

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)(pos_size));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void*)(pos_size + norm_size));

    glGenBuffers(1, &scene_->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene_->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.all_indices.size(),
                 data.all_indices.data(), GL_STATIC_DRAW);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
  }

  BufferData data_rigged = BufferData();
  auto character_model = LoadBufferMerge(characters_path, scene_->models_rigged, scene_->meshes_rigged, data_rigged, true);
  stbi_set_flip_vertically_on_load(false);
  LoadMaterials(characters_path, character_model, scene_->materials_rigged);
  stbi_set_flip_vertically_on_load(true);
  scene_->animator = std::move(character_model);

  glGenVertexArrays(1, &scene_->vao_rigged);
  glBindVertexArray(scene_->vao_rigged);
  glGenBuffers(1, &scene_->vbo_rigged);
  glBindBuffer(GL_ARRAY_BUFFER, scene_->vbo_rigged);

  size_t pos_size = data_rigged.all_positions.size();
  size_t norm_size = data_rigged.all_normals.size();
  size_t uv_size = data_rigged.all_uvs.size();
  size_t joint_size = data_rigged.all_joints.size();
  size_t weight_size = data_rigged.all_weights.size();

  size_t pos_offset = 0;
  size_t norm_offset = pos_size;
  size_t uv_offset = norm_offset + norm_size;
  size_t joint_offset = uv_offset + uv_size;
  size_t weight_offset = joint_offset + joint_size;

  size_t total_vbo_size = weight_offset + weight_size;

  glGenVertexArrays(1, &scene_->vao_rigged);
  glBindVertexArray(scene_->vao_rigged);

  glGenBuffers(1, &scene_->vbo_rigged);
  glBindBuffer(GL_ARRAY_BUFFER, scene_->vbo_rigged);
  glBufferData(GL_ARRAY_BUFFER, total_vbo_size, nullptr, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, pos_offset, pos_size, data_rigged.all_positions.data());
  glBufferSubData(GL_ARRAY_BUFFER, norm_offset, norm_size, data_rigged.all_normals.data());
  glBufferSubData(GL_ARRAY_BUFFER, uv_offset, uv_size, data_rigged.all_uvs.data());
  glBufferSubData(GL_ARRAY_BUFFER, joint_offset, joint_size, data_rigged.all_joints.data());
  glBufferSubData(GL_ARRAY_BUFFER, weight_offset, weight_size, data_rigged.all_weights.data());

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)pos_offset);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)norm_offset);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)uv_offset);
  glEnableVertexAttribArray(3);
  glVertexAttribIPointer(3, 4, data_rigged.joints_type, 0, (void*)joint_offset);
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (void*)weight_offset);

  glGenBuffers(1, &scene_->ebo_rigged);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene_->ebo_rigged);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_rigged.all_indices.size(),
               data_rigged.all_indices.data(), GL_STATIC_DRAW);

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);
  glVertexAttribDivisor(4, 0);

  glBindVertexArray(0);

  return scene_.get();
}

Scene::Type GetModelType(const tinygltf::Mesh& mesh) {
  auto type = Scene::Type::Static;
  if (mesh.extras.Has("body")) {
    std::string collision_value = mesh.extras.Get("body").Get<std::string>();
    if (collision_value == "dynamic") {
      type = Scene::Type::Dynamic;
    } else if (collision_value == "door") {
      type = Scene::Type::Door;
    } else if (collision_value == "light") {
      type = Scene::Type::PointLight;
    } else if (collision_value == "bench") {
      type = Scene::Type::Bench;
    }
  }
  return type;
}

Scene::CollisionType GetCollisionType(const tinygltf::Mesh& mesh) {
  auto type = Scene::CollisionType::Cube;
  if (mesh.extras.Has("collision")) {
    std::string collision_value = mesh.extras.Get("collision").Get<std::string>();
    if (collision_value == "sphere") {
      type = Scene::CollisionType::Sphere;
    } else if (collision_value == "capsule") {
      type = Scene::CollisionType::Capsule;
    } else if (collision_value == "cube") {
      type = Scene::CollisionType::Cube;
    }
  }
  return type;
}

const tinygltf::Accessor& LoadArrayBuffer(std::string_view name, const tinygltf::Model& model,
  const tinygltf::Primitive& primitive,
  std::vector<std::uint8_t>& data) {
  const auto& Accessor =
          model.accessors.at(primitive.attributes.at(name.data()));
  const auto& posView = model.bufferViews.at(Accessor.bufferView);
  const auto& posBuffer = model.buffers.at(posView.buffer);
  size_t posBytes = Accessor.count * Accessor.ByteStride(posView);
  const uint8_t* posStart =
      &posBuffer.data.at(0) + posView.byteOffset + Accessor.byteOffset;
  data.insert(data.end(), posStart,
                            posStart + posBytes);
  return Accessor;
}

const tinygltf::Accessor& LoadIndexBuffer(const tinygltf::Model& model,
  const tinygltf::Primitive& primitive,
  std::vector<std::uint8_t>& data) {
  const auto& idxAccessor = model.accessors.at(primitive.indices);
  const auto& idxView = model.bufferViews.at(idxAccessor.bufferView);
  const auto& idxBuffer = model.buffers.at(idxView.buffer);
  size_t idxElementSize =
      (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
          ? 4
          : 2;
  size_t padding =
      (idxElementSize - (data.size() % idxElementSize)) %
      idxElementSize;
  data.insert(data.end(), padding,
                          0);  // Apply padding bytes

  size_t idxBytes = idxAccessor.count * idxElementSize;
  const uint8_t* idxStart =
      &idxBuffer.data.at(0) + idxView.byteOffset + idxAccessor.byteOffset;

  data.insert(data.end(), idxStart, idxStart + idxBytes);
  return idxAccessor;
}

const tinygltf::Accessor& LoadJointsBuffer(const tinygltf::Model& model,
  const tinygltf::Primitive& primitive,
  std::vector<std::uint8_t>& data) {
  const auto& Accessor = model.accessors.at(primitive.attributes.at("JOINTS_0"));

  const auto& jointsView = model.bufferViews.at(Accessor.bufferView);
  const auto& jointsBuffer = model.buffers.at(jointsView.buffer);

  size_t componentSize = 1;

  if (Accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
    componentSize = 2;
  }

  size_t jointElementSize = 4 * componentSize;
  size_t jointsBytes = Accessor.count * jointElementSize;

  const uint8_t* jointsStart =
      &jointsBuffer.data.at(0) + jointsView.byteOffset + Accessor.byteOffset;

  data.insert(data.end(), jointsStart, jointsStart + jointsBytes);
  return Accessor;
}

tinygltf::Model ModelLoader::LoadBufferMerge(std::string_view path,
                                  std::vector<Scene::Model>& models,
                                  std::vector<Scene::Mesh>& meshes,
                                  BufferData& data,
                                  bool is_rigged) {
  std::string err, warn;
  tinygltf::Model model;
  bool res = loader_.LoadASCIIFromFile(&model, &err, &warn, path.data());
  if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
  if (!err.empty()) std::cout << "ERR: " << err << std::endl;
  if (!res) throw "Failed to load glTF";
  int prev_total_meshes = meshes.size();

  for (const auto& mesh : model.meshes) {
    int primitives_offset = meshes.size();
    glm::vec3 mesh_min(std::numeric_limits<float>::max());
    glm::vec3 mesh_max(std::numeric_limits<float>::min());
    for (const auto& primitive : mesh.primitives) {
      const auto& posAccessor = LoadArrayBuffer("POSITION", model, primitive, data.all_positions);
      glm::vec3 min(posAccessor.minValues[0], posAccessor.minValues[1],
                        posAccessor.minValues[2]);
      glm::vec3 max(posAccessor.maxValues[0], posAccessor.maxValues[1],
                    posAccessor.maxValues[2]);
      mesh_min = glm::min(mesh_min, min);
      mesh_max = glm::max(mesh_max, max);

      LoadArrayBuffer("NORMAL", model, primitive, data.all_normals);
      LoadArrayBuffer("TEXCOORD_0", model, primitive, data.all_uvs);

      if (is_rigged) {
        LoadArrayBuffer("WEIGHTS_0", model, primitive, data.all_weights);
        auto jointsAccessor = LoadJointsBuffer(model, primitive, data.all_joints);
        if (jointsAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
          data.joints_type = GL_UNSIGNED_SHORT;
        }
      }

      size_t index_byte_offset = data.all_indices.size();
      const auto& idxAccessor = LoadIndexBuffer(model, primitive, data.all_indices);
      GLenum gl_idx_type =
      (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
          ? GL_UNSIGNED_INT
          : GL_UNSIGNED_SHORT;

      int material_id = primitive.material;
      if (material_id == -1) {
        material_id = 0;
      }
      meshes.push_back({idxAccessor.count, index_byte_offset,
                        data.current_base_vertex, gl_idx_type, min, max,
                        material_id});
      data.current_base_vertex += posAccessor.count;
    }
    auto collision_type = GetCollisionType(mesh);
    auto model_type = GetModelType(mesh);
    models.push_back(
      {collision_type, model_type, mesh.name, primitives_offset,
      static_cast<int>(meshes.size() - primitives_offset),
      mesh_min, mesh_max, {}});
  }

  const auto gltf_scene = &model.scenes[model.defaultScene];
  for (size_t i = 0; i < gltf_scene->nodes.size(); ++i) {
    // assert((gltf_scene->nodes[i] >= 0) && (gltf_scene->nodes[i] <
    // model_data->model.nodes.size()));
    BindModelNodesScene(model, model.nodes[gltf_scene->nodes[i]], models,
                        prev_total_meshes);
  }
  return model;
}

void ModelLoader::BindModelNodesScene(tinygltf::Model& model,
                                      tinygltf::Node& node,
                                      std::vector<Scene::Model>& models,
                                      int idx_offset) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size()) &&
      !node.translation.empty()) {
    auto rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    auto scale = glm::vec3(1.0f);
    auto position = glm::vec3(node.translation[0], node.translation[1],
                              node.translation[2]);
    if (!node.rotation.empty()) {
      rotation = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1],
                           node.rotation[2]);
    }
    if (!node.scale.empty()) {
      scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
    }
    models[node.mesh + idx_offset].instances.push_back(
        {glm::vec4(1.0f), position, rotation, scale});
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    BindModelNodesScene(model, model.nodes[node.children[i]], models,
                        idx_offset);
  }
}

void ModelLoader::LoadMaterials(std::string_view path, tinygltf::Model& model,
                                std::vector<Material>& materials) {
  for (const auto& m : model.materials) {
    Material material;
    auto albedo_tex_id = m.pbrMetallicRoughness.baseColorTexture.index;
    if (albedo_tex_id == -1) {
      continue;
    }
    material.albedo = LoadTexture(path, model, albedo_tex_id);
    // auto normal_tex_id = m.normalTexture.index;
    // material.normal = LoadTexture(path, model, normal_tex_id);
    materials.push_back(std::move(material));
  }
  //  auto emission_tex_id =
  //      model.materials[0].emissiveTexture.index;
  //  model_data.material.emission = LoadTexture(model, emission_tex_id);
  //  auto metal_rough_tex_id =
  //      model.materials[0].pbrMetallicRoughness.metallicRoughnessTexture.index;
  //  model_data.material.metal_rough = LoadTexture(model, metal_rough_tex_id);
  //  auto normal_tex_id =
  //      model.materials[0].normalTexture.index;
  //  model_data.material.normal = LoadTexture(model, normal_tex_id);
  //  auto occlusion_tex_id =
  //      model.materials[0].occlusionTexture.index;
  //  model_data.material.occlusion = LoadTexture(model, occlusion_tex_id);
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
