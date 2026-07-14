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

  /// LoadDebugShapes BEFORE THE LoadScene,
  /// because in the LoadScene we init nodes with dbg_shape
  void LoadDebugShapes(std::string_view path);

  void LoadScene(std::string_view path);

  void LoadCharacters(std::string_view skeleton_path,
    std::vector<std::string_view> skin_paths);

  void LoadWeapon(std::vector<std::string_view> paths);

  struct BufferData {
    std::vector<std::uint8_t> all_positions;
    std::vector<std::uint8_t> all_normals;
    std::vector<std::uint8_t> all_uvs;
    std::vector<std::uint8_t> all_tangents;
    std::vector<std::uint8_t> all_indices;
    int current_base_vertex = 0;
  };

  struct BufferDataAnimated {
    std::vector<std::uint8_t> all_weights;
    std::vector<std::uint8_t> all_joints;
    GLenum joints_type = GL_UNSIGNED_BYTE;
  };

  Scene* GetScene() noexcept {
    return &scene_;
  }

 private:
  BufferData LoadBuffers(const tinygltf::Model& model,
    std::vector<Scene::Mesh>& meshes);
  BufferDataAnimated LoadBuffersAnimated(const tinygltf::Model& model);

  Material LoadMaterial(std::string_view path, tinygltf::Model& model,
    const tinygltf::Material& m);

  MaterialArray LoadMaterials(std::string_view path, tinygltf::Model& model);

  Texture LoadTexture(std::string_view path, const tinygltf::Model& model,
                      int tex_id);

  std::vector<uint8_t> LoadTextureRaw(
    std::string_view path, const tinygltf::Model& model, int tex_id);


  Scene::Skin LoadSkin(const tinygltf::Model& model);

  std::vector<Scene::Animation> LoadAnimations(
    const tinygltf::Model& model);

  tinygltf::TinyGLTF loader_;
  Scene scene_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_
