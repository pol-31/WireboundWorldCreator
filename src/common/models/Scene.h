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
#include "StaticObject.h"
#include "PointLight.h"

enum class CharacterAnimType {
  Idle,
  IdleNoArm,
  Climb,
  IdleCrouch,
  IdleCrouchNoArm,
  Fall,
  Holster,
  Hurt,
  Jump,
  Kick,
  MoveJump,
  Reload,
  Shoot,
  IdleSitting,
  Slide,
  WalkForward,
  WalkBackward,
  WalkLeft,
  WalkRight,
  RunForward,
  RunBackward,
  RunLeft,
  RunRight,
  CrouchForward,
  CrouchBackward,
  CrouchLeft,
  CrouchRight,
  PosesIdle,
  PosesPistol,

  kNone,
};

/// order specified in gltf (default alphabet by now)
enum class CharacterPoseDir {
  Center,
  Down,
  Left,
  Right,
  Up,
};

enum class WeaponAnimType {
  Idle,
  Shoot,
  Reload,
};

struct SceneNodePose {
  JPH::Vec3 t = JPH::Vec3::sZero();
  JPH::Quat r = JPH::Quat::sIdentity();
  JPH::Vec3 s = JPH::Vec3::sOne();

  JPH::Mat44 Matrix() const noexcept;
};

/// for animated there's no children and global_transform used as model_mat
struct SceneNode {
  std::vector<SceneNode*> children;
  SceneNode* parent = nullptr;
  JPH::BodyID body_id;

  int mesh_index = -1;
  const JPH::Shape* shape = nullptr;

  JPH::AABox bounds;
  // const, filled once (DO NOT OVERRIDE by animations)
  SceneNodePose local_transform;
  // used for global transform after the physics update
  JPH::Mat44 global_transform;
  int node_id = -1;
  bool can_be_activated = false;
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
    None,
    Dynamic,
    PointLight,
    Hinge,
    Door,
    Portal,
    Zone,
    Tile,
    Terrain,
    Character,
    Player,
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


  // upper body pose blend (neck / shoulders & neck)
  struct PoseDeltas {
    std::vector<SceneNodePose> center;
    std::vector<SceneNodePose> left;
    std::vector<SceneNodePose> right;
    std::vector<SceneNodePose> up;
    std::vector<SceneNodePose> down;
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
    int hips_id = 0;
    int spine0_id = 0;
    int spine1_id = 0;
    int spine2_id = 0;
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

  struct Portal {
    SceneNode* portal = nullptr; // area to pass
    SceneNode* render = nullptr; // e.g. door frame
    JPH::Vec3 position = JPH::Vec3::sZero();
    int connected_zone_index_1 = -1;
    int connected_zone_index_2 = -1;
  };
  struct Zone {
    std::vector<SceneNode*> object_nodes;
    std::vector<Portal*> portals;
    JPH::AABox bounds;
    std::string name;
    std::vector<PointLight> point_lights_;
    std::vector<StaticObject> static_objects_;
  };
  struct Tile {
    std::vector<Zone*> zones;
    std::vector<Portal> portals;
    std::vector<SceneNode*> characters;
    JPH::AABox bounds;
    std::string name;
    std::string hmap_path;
  };

  struct SceneData {
    std::vector<Mesh> meshes;

    std::vector<Tile*> tiles;
    SceneNode* player_node;

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
    std::vector<SceneNode*> nodes;
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
    std::vector<SceneNode*> nodes;
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

  void ConnectZonesWithPortals(int tile_id);
};

struct AnimatedRenderData {
  JPH::Mat44 transform;
  int bones_offset;
  GLuint vao;
  const Material* material;
  const std::vector<Scene::Mesh>* meshes;
};

#endif  // WIREBOUNDWORLDCREATOR_SCENE_H
