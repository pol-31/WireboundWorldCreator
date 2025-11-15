#include "ModelLoader.h"

bool LoadImageData(
    tinygltf::Image *image, const int image_idx, std::string *err,
    std::string *warn, int req_width, int req_height,
    const unsigned char *bytes, int size, void *user_data) {
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

void ModelData::BindTextures() const noexcept {
  glActiveTexture(GL_TEXTURE0);
  material.albedo.Bind();
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
  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
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
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos.at(indexAccessor.bufferView));
    glDrawElements(
        primitive.mode, indexAccessor.count, indexAccessor.componentType,
        ((char *)nullptr + (indexAccessor.byteOffset)));
  }
}

ModelLoader::ModelLoader(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources) {
  loader_.SetImageLoader(LoadImageData, nullptr);
}

ModelLoader::~ModelLoader() {
  //todo; reserve vaos
  for (auto& m : models_) {
    glDeleteVertexArrays(1, &m.vao);
  }
}

const ModelData* ModelLoader::Load(std::string_view path, int id) {
  ModelData model_data;
  ui_shared_resources_.shader_model_.Bind();
  glUniform1i(1, 0);
  glActiveTexture(GL_TEXTURE0);
  std::string err;
  std::string warn;
  bool res = loader_.LoadASCIIFromFile(
      &model_data.model, &err, &warn, path.data());
  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }
  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }
  if (!res) {
    throw "Failed to load glTF";
  } else {
    std::cout << "Loaded glTF: " << path << std::endl;
  }
  BindModel(model_data.model, model_data.vao, model_data.ebos);
  model_data.aabb = GetAabb(model_data.model);
  model_data.id = id;
  models_.push_back(std::move(model_data));
  return &models_.back();
}


void ModelLoader::BindMesh(
    tinygltf::Model& model, tinygltf::Mesh& mesh,
    std::map<int, GLuint>& ebos) {
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == 0) {  // TODO impl drawarrays
      std::cout << "WARN: bufferView.target is zero" << std::endl;
      continue;  // Unsupported bufferView.
                 /*
                   From spec2.0 readme:
                   https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
                            ... drawArrays function should be used with a count equal to
                   the count            property of any of the accessors referenced by the
                   attributes            property            (they are all equal for a given
                   primitive).
                 */
    }

    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
    std::cout << "bufferview.target " << bufferView.target << std::endl;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    ebos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);

    std::cout << "buffer.data.size = " << buffer.data.size()
              << ", bufferview.byteOffset = " << bufferView.byteOffset
              << std::endl;

    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    for (auto &attrib : primitive.attributes) {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byteStride =
          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
      glBindBuffer(GL_ARRAY_BUFFER, ebos[accessor.bufferView]);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }

      int vaa = -1;
      if (attrib.first.compare("POSITION") == 0) vaa = 0;
      if (attrib.first.compare("NORMAL") == 0) vaa = 1;
      if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
      if (vaa > -1) {
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(
            vaa, size, accessor.componentType,
            accessor.normalized ? GL_TRUE : GL_FALSE,
            byteStride, ((char *)nullptr + (accessor.byteOffset)));
      } else
        std::cout << "vaa missing: " << attrib.first << std::endl;
    }
  }
}

void ModelLoader::BindModelNodes(
    tinygltf::Model& model, tinygltf::Node &node,
    std::map<int, GLuint>& ebos) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    BindMesh(model, model.meshes[node.mesh], ebos);
  }

  for (size_t i = 0; i < node.children.size(); i++) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    BindModelNodes(model, model.nodes[node.children[i]], ebos);
  }
}

void ModelLoader::BindModel(
    tinygltf::Model& model, GLuint& vao, std::map<int, GLuint>& ebos) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
    BindModelNodes(model, model.nodes[scene.nodes[i]], ebos);
  }

  glBindVertexArray(0);
  // cleanup vbos but do not delete index buffers yet
  for (auto it = ebos.cbegin(); it != ebos.cend();) {
    tinygltf::BufferView bufferView = model.bufferViews[it->first];
    if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
      glDeleteBuffers(1, &ebos[it->first]);
      ebos.erase(it++);
    }
    else {
      ++it;
    }
  } // TODO: check that pretty interesting move with vbo removing
}

void ModelLoader::LoadTextures(ModelData& model_data) {
  tinygltf::Model& model = model_data.model;
  auto albedo_tex_id =
      model.materials[0].pbrMetallicRoughness.baseColorTexture.index;
  model_data.material.albedo = LoadTexture(model, albedo_tex_id);
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

Texture&& ModelLoader::LoadTexture(
    const tinygltf::Model& model, int tex_id) {
  if (tex_id == -1) {
    throw "model textures load failed 1";
  }
  auto& texture = model.textures[tex_id];
  auto image_index = texture.source;
  if (image_index == -1) {
    throw "model textures load failed";
  }
  auto& image_uri = model.images[image_index].uri;
  return Texture(image_uri, GL_RGBA);
}

Aabb3D ModelLoader::GetAabb(const tinygltf::Model& model) {
  glm::vec3 minBounds(FLT_MAX);
  glm::vec3 maxBounds(-FLT_MAX);

  for (const auto& mesh : model.meshes) {
    for (const auto& primitive : mesh.primitives) {
      auto it = primitive.attributes.find("POSITION");
      if (it == primitive.attributes.end()) continue;

      const tinygltf::Accessor& accessor = model.accessors[it->second];
      if (accessor.bufferView < 0 || accessor.bufferView >= model.bufferViews.size()) continue;

      if (!accessor.minValues.empty() && !accessor.maxValues.empty()) {
        glm::vec3 minV(
            static_cast<float>(accessor.minValues[0]),
            static_cast<float>(accessor.minValues[1]),
            static_cast<float>(accessor.minValues[2])
        );
        glm::vec3 maxV(
            static_cast<float>(accessor.maxValues[0]),
            static_cast<float>(accessor.maxValues[1]),
            static_cast<float>(accessor.maxValues[2])
        );

        minBounds = glm::min(minBounds, minV);
        maxBounds = glm::max(maxBounds, maxV);
      } else {
        const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        const unsigned char* dataPtr = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
        size_t stride = accessor.ByteStride(bufferView);
        if (stride == 0)
          stride = tinygltf::GetComponentSizeInBytes(accessor.componentType) * 3;

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
  Aabb3D aabb = {minBounds, maxBounds};
  return aabb;
}
