#ifndef WIREBOUNDWORLDCREATOR_SCENE_H
#define WIREBOUNDWORLDCREATOR_SCENE_H

#include <string>
#include <memory>
#include <vector>

#include <jolt/jolt.h>
#include <jolt/Physics/Collision/Shape/Shape.h>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../Material.h"
#include "Animator.h"

struct Scene {
  enum class CollisionType {
    Cube,
    Sphere,
    Capsule
  };

  enum class Type {
    Static,
    Dynamic,
    Door,
    PointLight,
    Bench,
    Zone,
  };

  /// Mesh is geometry with 1 material
  struct Mesh {
    size_t index_count;
    size_t index_byte_offset;
    int base_vertex;
    GLenum index_type;
    glm::vec3 min;
    glm::vec3 max;
    uint32_t material_id;

    void Render() const noexcept;
  };

  /// if we got a model consist of few materials, that means
  /// we have Model with vector<Mesh> which are single-materials geometry
  ///
  /// If we have few geometry data with same material, then
  /// our our meshes just have the same material id

  struct Instance {
    glm::vec4 color;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    // missing world bounds for culling
  };

  struct Model {
    CollisionType collision_type;
    Type type = Type::Static;
    std::string name; // dbg
    int primitives_offset = 0;
    int primitives_num = 0;
    glm::vec3 min;
    glm::vec3 max;
    std::vector<Instance> instances; // data collected from .gltf file (alt+l)
  };

  GLuint vao = 0;
  GLuint vbo = 0;
  GLuint ebo = 0;

  std::vector<Mesh> meshes;
  std::vector<Model> models;
  MaterialArray materials;

  // rigging (characters, weapon)

  GLuint vao_rigged = 0;
  GLuint vbo_rigged = 0;
  GLuint ebo_rigged = 0;

  std::vector<Mesh> meshes_rigged;
  std::vector<Model> models_rigged;
  Material material_character;
  Material material_gun;

  // moved inside the animator
  // GLuint ssbo_rigging = 0; // single large ssbo for everybody

  Animator animator; // character rigging

  void BindTextures() const noexcept;

  void RenderModelNodes() const;

  void RenderModelNode(const tinygltf::Node& node) const;

  void RenderMesh(const tinygltf::Mesh& mesh) const;

  void RenderPrimitive(int id) const;
};

#endif  // WIREBOUNDWORLDCREATOR_SCENE_H
