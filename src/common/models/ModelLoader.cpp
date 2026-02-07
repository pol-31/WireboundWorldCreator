#include "ModelLoader.h"

#include <stb_image.h>

#include <filesystem>

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

void ModelData::BindTextures() const noexcept {
  material.albedo.BindSampler(1);
  //  glActiveTexture(GL_TEXTURE1);
  //  material.emission.Bind();
  //  glActiveTexture(GL_TEXTURE2);
  //  material.metal_rough.Bind();
  //  glActiveTexture(GL_TEXTURE3);
  //  material.normal.Bind();
  //  glActiveTexture(GL_TEXTURE4);
  //  material.occlusion.Bind();
}

void ModelData::RenderModelNodes() const {
  const tinygltf::Scene& scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    RenderModelNode(model.nodes[scene.nodes[i]]);
  }
}

void ModelData::RenderModelNode(const tinygltf::Node& node) const {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    RenderMesh(model.meshes[node.mesh]);
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    RenderModelNode(model.nodes[node.children[i]]);
  }
}

void ModelData::RenderMesh(const tinygltf::Mesh& mesh) const {
  for (auto& prim : primitives) {
    glBindVertexArray(prim.vao);
    glDrawElements(prim.mode, prim.indexCount, prim.indexType,
                   ByteOffset(prim.indexOffset));
  }
}

void ModelData::RenderModelNodesInstanced(int instances_num) const {
  const tinygltf::Scene& scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    RenderModelNodeInstanced(model.nodes[scene.nodes[i]], instances_num);
  }
}

void ModelData::RenderModelNodeInstanced(const tinygltf::Node& node,
                                         int instances_num) const {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    RenderMeshInstanced(model.meshes[node.mesh], instances_num);
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    RenderModelNodeInstanced(model.nodes[node.children[i]], instances_num);
  }
}

void ModelData::RenderMeshInstanced(const tinygltf::Mesh& mesh,
                                    int instances_num) const {
  for (auto& prim : primitives) {
    glBindVertexArray(prim.vao);
    glDrawElementsInstanced(prim.mode, prim.indexCount, prim.indexType,
                            ByteOffset(prim.indexOffset), instances_num);
  }
}

ModelLoader::ModelLoader(UiRenderData& render_data,
                         tinygltf::TinyGLTF& loader)
    : render_data_(render_data), loader_(loader) {}

ModelLoader::~ModelLoader() {
  for (auto& model : models_) {
    for (auto& mesh : model->primitives) {
      glDeleteVertexArrays(1, &mesh.vao);
    }
  }
}

ModelData* ModelLoader::Load(std::string_view path, int id) {
  auto model_data = std::make_unique<ModelData>();
  std::string err, warn;
  bool res =
      loader_.LoadASCIIFromFile(&model_data->model, &err, &warn, path.data());
  if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
  if (!err.empty()) std::cout << "ERR: " << err << std::endl;

  if (!res)
    throw "Failed to load glTF";
  else
    std::cout << "Loaded glTF: " << path << std::endl;

  BindModel(model_data->model, model_data->primitives);
  model_data->aabb = GetAabb(model_data->model);
  model_data->name = std::filesystem::path(path).stem().string();

  // (important) extra rebind, otherwise messed triangles
  for (auto& p : model_data->primitives) {
    glBindVertexArray(p.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p.ebo);
  }
  glBindVertexArray(0);

  models_.push_back(std::move(model_data));
  stbi_set_flip_vertically_on_load(false);
  LoadTextures(path, models_.back().get());
  stbi_set_flip_vertically_on_load(true);

  return models_.back().get();
}

void ModelLoader::BindMesh(tinygltf::Model& model, tinygltf::Mesh& mesh,
                           std::map<int, GLuint>& ebos,
                           std::vector<ModelData::Mesh>& primitives) {
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView& bufferView = model.bufferViews[i];
    if (bufferView.target == 0) {
      // std::cout << "WARN: bufferView.target is zero" << std::endl;
      continue;
    }
    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    GLuint vbo;
    glGenBuffers(1, &vbo);
    ebos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);
    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    ModelData::Mesh cache_mesh;
    glGenVertexArrays(1, &cache_mesh.vao);
    glBindVertexArray(cache_mesh.vao);
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
    for (auto& attrib : primitive.attributes) {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byteStride =
          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
      glBindBuffer(GL_ARRAY_BUFFER, ebos[accessor.bufferView]);
      int vaa = -1;
      int size = 3;
      if (attrib.first == "POSITION") {
        vaa = 0;
        size = 3;
      } else if (attrib.first == "NORMAL") {
        vaa = 1;
        size = 3;
      } else if (attrib.first == "TEXCOORD_0") {
        vaa = 2;
        size = 2;
      } else if (attrib.first == "JOINTS_0") {
        vaa = 3;
        size = 4;
      } else if (attrib.first == "WEIGHTS_0") {
        vaa = 4;
        size = 4;
      }

      if (vaa == -1) {
        std::cout << "vaa missing: " << attrib.first << std::endl;
        continue;
      }
      glEnableVertexAttribArray(vaa);
      GLenum type = accessor.componentType;
      if (attrib.first == "JOINTS_0") {
        if (type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
          type = GL_UNSIGNED_BYTE;
        } else if (type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
          type = GL_UNSIGNED_SHORT;
        }
        glVertexAttribIPointer(vaa, size, type, byteStride,
                               ByteOffset(accessor.byteOffset));
      } else {
        glVertexAttribPointer(vaa, size, type,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, ByteOffset(accessor.byteOffset));
      }
    }

    GLuint ebo = ebos[indexAccessor.bufferView];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    cache_mesh.ebo = ebo;
    cache_mesh.mode = primitive.mode;
    cache_mesh.indexCount = indexAccessor.count;
    cache_mesh.indexType = indexAccessor.componentType;
    cache_mesh.indexOffset = indexAccessor.byteOffset;
    primitives.push_back(cache_mesh);
    glBindVertexArray(0);
  }
}

void ModelLoader::BindModelNodes(tinygltf::Model& model, tinygltf::Node& node,
                                 std::map<int, GLuint>& ebos,
                                 std::vector<ModelData::Mesh>& primitives) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    BindMesh(model, model.meshes[node.mesh], ebos, primitives);
  }

  for (size_t i = 0; i < node.children.size(); i++) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    BindModelNodes(model, model.nodes[node.children[i]], ebos, primitives);
  }
}

void ModelLoader::BindModel(tinygltf::Model& model,
                            std::vector<ModelData::Mesh>& primitives) {
  std::map<int, GLuint> ebos;
  const tinygltf::Scene& scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
    BindModelNodes(model, model.nodes[scene.nodes[i]], ebos, primitives);
  }
  // cleanup vbos but do not delete index buffers yet
  for (auto it = ebos.cbegin(); it != ebos.cend();) {
    tinygltf::BufferView bufferView = model.bufferViews[it->first];
    if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
      glDeleteBuffers(1, &ebos[it->first]);
      ebos.erase(it++);
    } else {
      ++it;
    }
  }  // TODO: check that pretty interesting move with vbo removing
}

void ModelLoader::LoadTextures(std::string_view path, ModelData* model_data) {
  tinygltf::Model& model = model_data->model;
  if (model.materials.empty()) {
    return;
  }
  auto albedo_tex_id =
      model.materials[0].pbrMetallicRoughness.baseColorTexture.index;
  model_data->material.albedo = LoadTexture(path, model, albedo_tex_id);
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
  return Texture(tex_path.string(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
}

Aabb3D ModelLoader::GetAabb(const tinygltf::Model& model) {
  glm::vec3 minBounds(FLT_MAX);
  glm::vec3 maxBounds(-FLT_MAX);

  for (const auto& mesh : model.meshes) {
    for (const auto& primitive : mesh.primitives) {
      auto it = primitive.attributes.find("POSITION");
      if (it == primitive.attributes.end()) continue;

      const tinygltf::Accessor& accessor = model.accessors[it->second];
      if (accessor.bufferView < 0 ||
          accessor.bufferView >= model.bufferViews.size())
        continue;

      if (!accessor.minValues.empty() && !accessor.maxValues.empty()) {
        glm::vec3 minV(static_cast<float>(accessor.minValues[0]),
                       static_cast<float>(accessor.minValues[1]),
                       static_cast<float>(accessor.minValues[2]));
        glm::vec3 maxV(static_cast<float>(accessor.maxValues[0]),
                       static_cast<float>(accessor.maxValues[1]),
                       static_cast<float>(accessor.maxValues[2]));

        minBounds = glm::min(minBounds, minV);
        maxBounds = glm::max(maxBounds, maxV);
      } else {
        const tinygltf::BufferView& bufferView =
            model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        const unsigned char* dataPtr =
            buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
        size_t stride = accessor.ByteStride(bufferView);
        if (stride == 0)
          stride =
              tinygltf::GetComponentSizeInBytes(accessor.componentType) * 3;

        for (size_t i = 0; i < accessor.count; ++i) {
          const float* v = reinterpret_cast<const float*>(dataPtr + stride * i);
          glm::vec3 pos(v[0], v[1], v[2]);
          minBounds = glm::min(minBounds, pos);
          maxBounds = glm::max(maxBounds, pos);
        }
      }
    }
  }
  if (minBounds.x == FLT_MAX) {
    throw "model aabb gen failed, no min max";
  }
  std::cout << "AABB:" << std::endl;
  std::cout << minBounds.x << ' ' << maxBounds.x << std::endl;
  std::cout << minBounds.y << ' ' << maxBounds.y << std::endl;
  std::cout << minBounds.z << ' ' << maxBounds.z << std::endl;
  Aabb3D aabb = {minBounds, maxBounds};
  glm::vec3 sizes = (aabb.max - aabb.min) / 2.0f;
  glm::vec3 center = (aabb.min + aabb.max) / 2.0f;
  sizes.x = glm::min(sizes.x, sizes.z);
  sizes.z = sizes.x;
  aabb.max = center + sizes;
  aabb.min = center - sizes;
  return aabb;
}
