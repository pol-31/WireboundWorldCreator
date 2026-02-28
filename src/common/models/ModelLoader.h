#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_

#include <glad/glad.h>
#include <tiny_gltf.h>

#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "../../render/Texture.h"
#include "../../ui/UiRenderData.h"
#include "../Material.h"
#include "Aabb3D.h"

bool LoadImageData(tinygltf::Image* image, const int image_idx,
                   std::string* err, std::string* warn, int req_width,
                   int req_height, const unsigned char* bytes, int size,
                   void* user_data);

struct ModelData {
  struct Primitive {
    GLuint vao = 0;
    GLuint ebo = 0;  // + vbo todo; deleted?
    GLenum mode = 0;
    GLsizei indexCount = 0;
    GLenum indexType = 0;
    size_t indexOffset = 0;
    GLenum usage = GL_STATIC_DRAW;
    void Render() const noexcept;
  };
  std::vector<Primitive> primitives;
  tinygltf::Model model;
  Material material;
  Aabb3D aabb;
  std::string name;

  float hp = 100.0f;
  float speed = 1.0f;
  float attack = 1.0f;
  float attack_speed = 1.0f;

  void BindTextures() const noexcept;

  void RenderModelNodes() const;

  void RenderModelNode(const tinygltf::Node& node) const;

  void RenderMesh(const tinygltf::Mesh& mesh) const;
};

class ModelLoader {
 public:
  ModelLoader(UiRenderData& render_data, tinygltf::TinyGLTF& loader);

  ~ModelLoader();

  ModelData* Load(std::string_view path, int id);

  std::vector<std::unique_ptr<ModelData>>& GetLoadedModels() { return models_; }

 private:
  void BindMesh(tinygltf::Model& model, tinygltf::Mesh& mesh,
                std::map<int, GLuint>& ebos,
                std::vector<ModelData::Primitive>& primitives);

  void BindModelNodes(tinygltf::Model& model, tinygltf::Node& node,
                      std::map<int, GLuint>& ebos,
                      std::vector<ModelData::Primitive>& primitives);

  void BindModel(tinygltf::Model& model,
                 std::vector<ModelData::Primitive>& primitives);

  // ptr (store uniq ptrs)
  void LoadTextures(std::string_view path, ModelData* model_data);

  Texture LoadTexture(std::string_view path, const tinygltf::Model& model,
                      int tex_id);

  Aabb3D GetAabb(const tinygltf::Model& model);

  std::vector<std::unique_ptr<ModelData>> models_;
  UiRenderData& render_data_;
  tinygltf::TinyGLTF& loader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_MODELLOADER_H_
