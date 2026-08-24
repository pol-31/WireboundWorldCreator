#ifndef WIREBOUNDWORLDCREATOR_SCENE_H
#define WIREBOUNDWORLDCREATOR_SCENE_H

#include <string>
#include <memory>
#include <vector>

#include <jolt/jolt.h>
#include <jolt/Physics/Body/BodyLockInterface.h>
#include <jolt/Physics/Collision/Shape/Shape.h>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../Material.h"

enum class CharacterAnimType {
  Idle,
  IdleSitting,
  Walk,
  Run,
  Crouch,
  Kick,
  Stunned,
  Jump,
  Fall,
  Slide,
  Climb,
  Throw,
  Swim,
  Shoot,

  IdleCenter,
  IdleDown,
  IdleLeft,
  IdleRight,
  IdleUp,

  PistolIdle,

  PistolIdleCenter,
  PistolIdleDown,
  PistolIdleLeft,
  PistolIdleRight,
  PistolIdleUp,

  PistolJump,
  PistolKneelIdle,
  PistolKneelToSit,
  PistolKneelToStand,
  PistolRun,
  PistolRunBackward,
  PistolStrifeLeft,
  PistolStrifeRight,
  PistolWalkBackward,
  PistolWalkForward,


  kNone,
};

enum class WeaponAnimType {
  Idle,
  Shoot,
  Reload,
};

struct Scene {
  enum class CollisionType {
    Cube,
    Sphere,
    Capsule,
    Cylinder
  };

  enum class Type {
    Static,
    Dynamic,
    PointLight,
    Hinge,
    Zone,
    Terrain,
    Character,
    Player,
    Tile,
  };

  /// idk, let's separate by Blender obj names
  enum class CharacterType {
    Default,
  };

  struct AnimationChannel {
    int sampler = -1;
    int target_node = -1;
    std::string target_path;
  };

  struct AnimationSampler {
    std::vector<float> times;
    std::vector<float> values;
    std::string interpolation;
  };

  struct Animation {
    std::string name;
    std::vector<AnimationChannel> channels;
    std::vector<AnimationSampler> samplers;
  };

  struct Joint {
    int node = -1;               // index into model.nodes
    JPH::Mat44 inverseBind;  // from glTF
  };

  struct NodePose {
    JPH::Vec3 t = JPH::Vec3::sZero();
    JPH::Quat r = JPH::Quat::sIdentity();
    JPH::Vec3 s = JPH::Vec3::sOne();

    JPH::Mat44 Matrix() const noexcept;
  };

  // upper body pose blend (neck / shoulders & neck)
  struct PoseDeltas {
    std::vector<NodePose> left;
    std::vector<NodePose> right;
    std::vector<NodePose> up;
    std::vector<NodePose> down;
  };

  struct CoreRig {
    int skeletonRoot = -1;  // node index or -1
    std::vector<Joint> joints;
    std::vector<Animation> animations;
  };

  struct CharacterRig {
    CoreRig core_rig;
    int head_bone_id = 0;
    int hand_bone_id = 0;
    std::map<CharacterAnimType, int> mapping;
    PoseDeltas default_deltas;
    PoseDeltas pistol_deltas;
  };

  struct WeaponRig {
    CoreRig core_rig;
    std::map<WeaponAnimType, int> mapping;
  };

  /// Primitive is geometry with 1 material; loaded once, immutable
  struct Primitive {
    size_t index_count;
    size_t index_byte_offset;
    int base_vertex;
    GLenum index_type;
    uint32_t material_id;
  };

  struct Mesh {
    std::vector<Primitive> primitives;
    Type type = Type::Static;
    glm::vec3 min;
    glm::vec3 max;
    CollisionType collision_type; // defines how to reinterpret in dbg render
  };

  /// for animated there's no children and global_transform used as model_mat
  struct ModelNode {
    std::vector<ModelNode*> children;
    ModelNode* parent = nullptr;
    JPH::BodyID body_id;

    int mesh_index = -1;
    const JPH::Shape* shape = nullptr;

    JPH::AABox bounds;
    // const, filled once (DO NOT OVERRIDE by animations)
    NodePose local_transform;
    // used for global transform after the physics update
    JPH::Mat44 global_transform;
    int node_id = -1;
  };
  struct Zone {
    std::vector<ModelNode*> object_nodes;
    JPH::AABox bounds;
    std::string name;
  };
  struct Tile {
    std::vector<ModelNode*> object_nodes;
    std::vector<Zone*> zones;
    JPH::AABox bounds;
    std::string name;
    std::string hmap_path;
  };

  struct SceneData {
    std::vector<Mesh> meshes;

    std::vector<Tile*> tiles;
    ModelNode* player_node;

    // std::vector<ModelNode*> nodes;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
  };

  MaterialArray materials; // shared for all scenes, for DBG use just index=0
  SceneData scene_data_;
  SceneData scene_dbg_shapes_;

  struct CharacterData {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    Material material;
    std::vector<Mesh> meshes;
    std::vector<ModelNode*> nodes;
    std::string name;
    std::vector<int> render_nodes;
  };
  std::vector<CharacterData> character_skins_;
  CharacterRig character_rig_; // same shared rigging for all characters

  struct WeaponData {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    WeaponRig rig;
    std::vector<Mesh> meshes;
    std::vector<ModelNode*> nodes;
    Material material;
    std::string name;

    float damage = 1.0f;
    float reload_speed = 1.0f;
    int bullets_max = 7;
    int bullets_total = 21;
    std::string mesh_name;
  };
  std::vector<WeaponData> weapons_;

  void UpdateRenderTransform(const JPH::BodyLockInterface& bli,
  std::vector<Tile*>& tiles);
};

struct AnimatedRenderData {
  JPH::Mat44 transform;
  int bones_offset;
  GLuint vao;
  const Material* material;
  const std::vector<Scene::Mesh>* meshes;
};

#endif  // WIREBOUNDWORLDCREATOR_SCENE_H
