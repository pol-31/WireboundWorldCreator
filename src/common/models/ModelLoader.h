#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_

#include <map>
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "../../render/Texture.h"
#include "Scene.h"

bool LoadImageData(tinygltf::Image* image, const int image_idx,
                   std::string* err, std::string* warn, int req_width,
                   int req_height, const unsigned char* bytes, int size,
                   void* user_data);

class ModelLoader {
 public:
  ModelLoader();

  ~ModelLoader();

  Scene* LoadScene(
    std::string_view collisions_path,
    std::string_view characters_path,
    std::string_view scene_path);

  struct BufferData {
    std::vector<std::uint8_t> all_positions;
    std::vector<std::uint8_t> all_normals;
    std::vector<std::uint8_t> all_uvs;
    std::vector<std::uint8_t> all_weights;
    std::vector<std::uint8_t> all_joints;
    std::vector<std::uint8_t> all_indices;
    int current_base_vertex = 0;
    GLenum joints_type = GL_UNSIGNED_BYTE;
  };

 private:
  // idx_offset inside meshes vector, so collisions has 0, scene has 4
  void BindModelNodesScene(tinygltf::Model& model, tinygltf::Node& node,
    std::vector<Scene::Model>& models, int idx_offset);

  /// is_rigged separates 3-component vbo from 5-component (weights, joints)
  tinygltf::Model LoadBufferMerge(std::string_view path,
    std::vector<Scene::Model>& primitives,
    std::vector<Scene::Mesh>& meshes,
    BufferData& data,
    bool is_rigged);

  // void BindMesh(tinygltf::Model& model, tinygltf::Mesh& mesh,
  //               std::map<int, GLuint>& ebos,
  //               std::vector<ModelData::Primitive>& primitives,
  //                          std::vector<ModelData::Model>& models);
  //
  // void BindModelNodes(tinygltf::Model& model, tinygltf::Node& node,
  //                     std::map<int, GLuint>& ebos,
  //                     std::vector<ModelData::Primitive>& primitives,
  //                          std::vector<ModelData::Model>& models);
  //
  // void BindModel(tinygltf::Model& model,
  // std::vector<ModelData::Primitive>& primitives,
  //           std::vector<ModelData::Model>& models);

  // ptr (store uniq ptrs)
  void LoadMaterials(std::string_view path, tinygltf::Model& model,
    std::vector<Material>& materials);

  Texture LoadTexture(std::string_view path, const tinygltf::Model& model,
                      int tex_id);

  tinygltf::TinyGLTF loader_;
  std::unique_ptr<Scene> scene_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_
