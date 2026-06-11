#include "ModelLoader.h"

#include <stb_image.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Animator.h"

const int materialWidth = 2048;
const int materialHeight = 2048;

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

ModelLoader::ModelLoader() { loader_.SetImageLoader(LoadImageData, nullptr); }

ModelLoader::~ModelLoader() {
  // for (auto& model : models_) {
  //   for (auto& mesh : model->primitives) {
  //     glDeleteVertexArrays(1, &mesh.vao);
  //   }
  // }
}

void ModelLoader::LoadScene(std::string_view collisions_path,
                            std::string_view characters_path,
                            std::string_view weapon_path,
                            std::string_view scene_path) {
  scene_ = std::make_unique<Scene>();
  {
    BufferData data;
    LoadBufferMerge(collisions_path, scene_->models, scene_->meshes, data,
                    false);

    {
      auto scene_model = LoadBufferMerge(scene_path, scene_->models,
                                         scene_->meshes, data, false);
      stbi_set_flip_vertically_on_load(false);
      scene_->materials = LoadMaterials(scene_path, scene_model);
      stbi_set_flip_vertically_on_load(true);
    }

    glGenVertexArrays(1, &scene_->vao);
    glBindVertexArray(scene_->vao);
    glGenBuffers(1, &scene_->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, scene_->vbo);

    size_t pos_size = data.all_positions.size();
    size_t norm_size = data.all_normals.size();
    size_t uv_size = data.all_uvs.size();
    size_t tangent_size = data.all_tangents.size();

    // 1. Calculate proper byte offsets sequentially
    size_t pos_offset = 0;
    size_t norm_offset = pos_offset + pos_size;
    size_t uv_offset = norm_offset + norm_size;
    size_t tangent_offset = uv_offset + uv_size;

    size_t total_vbo_size = tangent_offset + tangent_size;

    glBufferData(GL_ARRAY_BUFFER, total_vbo_size, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, pos_offset, pos_size,
                    data.all_positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, norm_offset, norm_size,
                    data.all_normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, uv_offset, uv_size, data.all_uvs.data());
    glBufferSubData(GL_ARRAY_BUFFER, tangent_offset, tangent_size,
                    data.all_tangents.data());

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)pos_offset);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)norm_offset);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)uv_offset);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)tangent_offset);

    glGenBuffers(1, &scene_->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene_->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.all_indices.size(),
                 data.all_indices.data(), GL_STATIC_DRAW);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glVertexAttribDivisor(3, 0);
  }

  BufferData data_rigged = BufferData();
  auto character_model =
      LoadBufferMerge(characters_path, scene_->models_rigged,
                      scene_->meshes_rigged, data_rigged, true);
  auto gun_model = LoadBufferMerge(weapon_path, scene_->models_rigged,
                                   scene_->meshes_rigged, data_rigged, true);
  stbi_set_flip_vertically_on_load(false);
  scene_->material_character = LoadMaterial(characters_path, character_model,
                                            character_model.materials[0]);
  scene_->material_gun =
      LoadMaterial(weapon_path, gun_model, gun_model.materials[0]);
  stbi_set_flip_vertically_on_load(true);
  scene_->animator = Animator(std::move(character_model), std::move(gun_model));

  glGenVertexArrays(1, &scene_->vao_rigged);
  glBindVertexArray(scene_->vao_rigged);
  glGenBuffers(1, &scene_->vbo_rigged);
  glBindBuffer(GL_ARRAY_BUFFER, scene_->vbo_rigged);

  size_t pos_size = data_rigged.all_positions.size();
  size_t norm_size = data_rigged.all_normals.size();
  size_t uv_size = data_rigged.all_uvs.size();
  size_t tangent_size = data_rigged.all_tangents.size();
  size_t joint_size = data_rigged.all_joints.size();
  size_t weight_size = data_rigged.all_weights.size();

  size_t pos_offset = 0;
  size_t norm_offset = pos_size;
  size_t uv_offset = norm_offset + norm_size;
  size_t tangent_offset = uv_offset + uv_size;
  size_t joint_offset = tangent_offset + tangent_size;
  size_t weight_offset = joint_offset + joint_size;

  size_t total_vbo_size = weight_offset + weight_size;

  glGenVertexArrays(1, &scene_->vao_rigged);
  glBindVertexArray(scene_->vao_rigged);

  glGenBuffers(1, &scene_->vbo_rigged);
  glBindBuffer(GL_ARRAY_BUFFER, scene_->vbo_rigged);
  glBufferData(GL_ARRAY_BUFFER, total_vbo_size, nullptr, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, pos_offset, pos_size,
                  data_rigged.all_positions.data());
  glBufferSubData(GL_ARRAY_BUFFER, norm_offset, norm_size,
                  data_rigged.all_normals.data());
  glBufferSubData(GL_ARRAY_BUFFER, uv_offset, uv_size,
                  data_rigged.all_uvs.data());
  glBufferSubData(GL_ARRAY_BUFFER, tangent_offset, tangent_size,
                  data_rigged.all_tangents.data());
  glBufferSubData(GL_ARRAY_BUFFER, joint_offset, joint_size,
                  data_rigged.all_joints.data());
  glBufferSubData(GL_ARRAY_BUFFER, weight_offset, weight_size,
                  data_rigged.all_weights.data());

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)pos_offset);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)norm_offset);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)uv_offset);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)tangent_offset);
  glEnableVertexAttribArray(4);
  glVertexAttribIPointer(4, 4, data_rigged.joints_type, 0, (void*)joint_offset);
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, (void*)weight_offset);

  glGenBuffers(1, &scene_->ebo_rigged);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene_->ebo_rigged);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_rigged.all_indices.size(),
               data_rigged.all_indices.data(), GL_STATIC_DRAW);

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 0);
  glVertexAttribDivisor(2, 0);
  glVertexAttribDivisor(3, 0);
  glVertexAttribDivisor(4, 0);
  glVertexAttribDivisor(5, 0);

  glBindVertexArray(0);
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
    } else if (collision_value == "zone") {
      type = Scene::Type::Zone;
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

tinygltf::Model ModelLoader::LoadBufferMerge(std::string_view path,
                                             std::vector<Scene::Model>& models,
                                             std::vector<Scene::Mesh>& meshes,
                                             BufferData& data, bool is_rigged) {
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

      if (is_rigged) {
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
                        static_cast<uint32_t>(material_id)});
      data.current_base_vertex += posAccessor.count;
    }
    auto collision_type = GetCollisionType(mesh);
    auto model_type = GetModelType(mesh);
    models.push_back({collision_type,
                      model_type,
                      mesh.name,
                      primitives_offset,
                      static_cast<int>(meshes.size() - primitives_offset),
                      mesh_min,
                      mesh_max,
                      {}});
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
