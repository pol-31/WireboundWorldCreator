#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_

#include <map>
#include <memory>
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "../Texture.h"
#include "../Material.h"
#include "../../modes/UiSharedResources.h"

// local space (rel to 0;0;0 centre)
struct Aabb3D {
  glm::vec3 min;
  glm::vec3 max;
};

bool LoadImageData(
    tinygltf::Image *image, const int image_idx, std::string *err,
    std::string *warn, int req_width, int req_height,
    const unsigned char *bytes, int size, void *user_data);

struct ModelData {
  enum class Category {
    kPlayer,
    kEnemy,
    kFriend,
    kNeutral,
    kObstacle
  };
  Category category = Category::kEnemy;
  float hp = 100.0f;
  float speed = 1.0f;
  float attack = 1.0f;
  float attack_speed = 1.0f;

  tinygltf::Model model;
  GLuint vao = 0;
  std::map<int, GLuint> ebos;
  GLuint id = 0;

  Material material;
  Aabb3D aabb;

  void BindTextures() const noexcept;

  void RenderModelNodes() const;

  void RenderModelNode(const tinygltf::Node& node) const;

  void RenderMesh(const tinygltf::Mesh& mesh) const;

  /// instanced
  void RenderModelNodesInstanced(int instances_num) const;

  void RenderModelNodeInstanced(
    const tinygltf::Node& node, int instances_num) const;

  void RenderMeshInstanced(
    const tinygltf::Mesh& mesh, int instances_num) const;
};

class ModelLoader {
 public:
  ModelLoader(UiSharedResources& ui_shared_resources);

  ~ModelLoader();

  const ModelData* Load(std::string_view path, int id);

  const std::vector<std::unique_ptr<ModelData>>& GetLoadedModels() {
    return models_;
  }

 private:
  void BindMesh(
      tinygltf::Model& model, tinygltf::Mesh& mesh,
      std::map<int, GLuint>& ebos);

  void BindModelNodes(
      tinygltf::Model& model, tinygltf::Node &node,
      std::map<int, GLuint>& ebos);

  void BindModel(
      tinygltf::Model& model, GLuint& vao,
      std::map<int, GLuint>& ebos);

  // ptr (store uniq ptrs)
  void LoadTextures(std::string_view path, ModelData* model_data);

  Texture LoadTexture(
    std::string_view path, const tinygltf::Model& model, int tex_id);

  Aabb3D GetAabb(const tinygltf::Model& model);

  std::vector<std::unique_ptr<ModelData>> models_;
  UiSharedResources& ui_shared_resources_;
  tinygltf::TinyGLTF loader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_
