#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_

#include <map>
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "../Texture.h"
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
  enum class TraitsCategory {
    kPlayer,
    kObstacle,
    kCreature,
  }; // 7 (early Faithful/Wirebound)

  enum class TraitsType {
    kPlayer,
    kFly,
    kTree
  }; // all models (I guess...)

  struct Material {
    /**
   * Because of astc encoding format we always have RGBA; so to store
   * rough_metal or normal map we do (official astc-enc recommendations):
   * """
   * To encode this we need to store only two input components
   * in the compressed data, and therefore use the rrrg coding swizzle
   * to align the data with the ASTC luminance+alpha endpoint.
   * """
   * So we decided to represent material as:
   * . albedo - rgba;
   * . metallic & roughness - as rrrg coding swizzle;
   * . normal - as rrrg coding swizzle;
   * - occlusion - rrrr
   * . emission - rgba
   * TODO: occlusion & emission can be stored in the same RGBA texture
     */
    Texture albedo;
    Texture emission;
    Texture metal_rough;
    Texture normal;
    Texture occlusion;
  };

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
};

class ModelLoader {
 public:
  ModelLoader(UiSharedResources& ui_shared_resources);

  ~ModelLoader();

  const ModelData* Load(std::string_view path, int id);

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

  void LoadTextures(ModelData& model_data);

  Texture&& LoadTexture(const tinygltf::Model& model, int tex_id);

  Aabb3D GetAabb(const tinygltf::Model& model);

  std::vector<ModelData> models_;
  UiSharedResources& ui_shared_resources_;
  tinygltf::TinyGLTF loader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_
