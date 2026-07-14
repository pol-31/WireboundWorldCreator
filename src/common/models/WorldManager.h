#ifndef WIREBOUNDWORLDCREATOR_WORLDMANAGER_H
#define WIREBOUNDWORLDCREATOR_WORLDMANAGER_H

#include <vector>
#include <memory>

#include "Scene.h"
#include "../../core/Frustum.h"

class Camera;
class PlayerController;
class EnemyController;
class Weapon;
class PointLight;
class DirectedLight;
class StaticObject;

/// I wanted to name it ZoneCuller, because the main job is to get
/// all object, cull them based on our zone (or camera frustum ???)
/// and put to another structures, so then we feed it to our Renderer
class WorldManager {
public:
  /// std430 layout (16-byte alignment)
  struct InstanceGpu {
    JPH::Mat44 model = JPH::Mat44::sIdentity();
    JPH::Vec4 color = JPH::Vec4::sOne();
    uint32_t material_id = 0;
    uint32_t padding1 = 0;
    uint32_t padding2 = 0;
    uint32_t padding3 = 0;
  };

  /// all needed for glDrawElementsInstancedBaseVertexBaseInstance
  struct SsboOffsetObject {
    int instance_offset = 0;
    int instances_num = 0;
    int mesh_id = 0;
  };

  WorldManager(const Scene* scene,
  const Camera* camera,
  const std::unique_ptr<PlayerController>& player,
  const std::vector<std::unique_ptr<EnemyController>>& characters,
  const std::vector<std::unique_ptr<Weapon>>& weapons,
  const std::vector<PointLight>& point_lights,
  const std::vector<DirectedLight>& dir_lights,
  const std::vector<StaticObject>& static_objects);

  void Cull();

private:
  // struct Zone {
  //   Scene::ModelNode* scene_node = nullptr;
  //   std::vector<int> point_lights;
  //   std::vector<int> dir_lights;
  //   std::vector<int> characters;
  //   std::vector<int> weapons;
  //   std::vector<int> objects;
  // };
  //
  // struct Tile {
  //   std::string_view hmap_path;
  //   JPH::AABox bounds;
  // };

  // TileRenderer tile_renderer_; // hmaps geoclipmaps

  /// this is our ALL objects from the scene, so
  /// there might be hundreds of objects (like 10 filled rooms),
  /// even 32 up to 100 point lights and up to 5 directed (sun and opt point l)
  ///
  /// Thus we can't create light per mesh vectors and keep them all the time.
  /// Instead...

  /// --- 1---
  const Scene* scene_;
  const Camera* camera_; // to know is it 1st/3rd AND frustum
  const std::unique_ptr<PlayerController>& player_; // TODO: only for shadows
  const std::vector<std::unique_ptr<EnemyController>>& characters_;
  const std::vector<std::unique_ptr<Weapon>>& weapons_;
  const std::vector<PointLight>& point_lights_;
  const std::vector<DirectedLight>& dir_lights_;
  const std::vector<StaticObject>& static_objects_;
  // std::vector<Zone> zones_;
  // int player_zone_ = 0;
  // std::vector<Tile> tiles_;
  // int player_tile_ = -1;
  Scene::Zone* cur_zone_ = nullptr;
  Scene::Tile* cur_tile_ = nullptr;

  /// TEMP --- 2 --- (not packed, but culled)
  /// Cull() before - all cleared
  /// Cull() start - only by zones (so source set)
  /// Cull() end - all data filled
  struct FrustumCulledObjects {
    std::vector<int> objects;
    std::vector<int> weapons;
    std::vector<int> characters;
    bool render_player = false; //1st 3rd for camera frustum
  };
  void PushFrustumCulled(const std::vector<int>& objects,
    std::vector<std::vector<InstanceGpu>>& ssbo_data);

  // resized with zone lights num, so its array idx == (int)cur_zone.light,
  // so we don't need to store ptr to the source
  std::vector<FrustumCulledObjects> active_point_lights_;
  std::vector<FrustumCulledObjects> active_dir_lights_;
  FrustumCulledObjects active_camera_;

  /// --- 3 ---
  struct FinalPointLight {
    const PointLight* source = nullptr;
    /// objects, that cast a shadow
    std::vector<SsboOffsetObject> objects; // by mesh, * is on gpu
    std::vector<std::unique_ptr<Weapon>*> weapons;
    std::vector<std::unique_ptr<EnemyController>*> characters;
    bool render_player = false;
  };
  struct FinalDirLight {
    const DirectedLight* source = nullptr;
    /// objects, that cast a shadow
    std::vector<SsboOffsetObject> objects;
    std::vector<std::unique_ptr<Weapon>*> weapons;
    std::vector<std::unique_ptr<EnemyController>*> characters;
    bool render_player = false;
  };
  struct FinalCamera {
    std::vector<SsboOffsetObject> objects;
    std::vector<std::unique_ptr<Weapon>*> weapons;
    std::vector<std::unique_ptr<EnemyController>*> characters;
  };

public:
  /// output (just --- 3 --- struct):
  struct ZoneCulledData {
    std::vector<InstanceGpu> ssbo_data;
    std::vector<FinalPointLight> point_lights;
    std::vector<FinalDirLight> dir_lights;
    FinalCamera camera;
  };

  const ZoneCulledData* GetCulledData() {
    return &final_render_data_;
  }

private:
  ZoneCulledData final_render_data_;
};

//TODO : get back PointLight DirLight to lightweight structures

#endif  // WIREBOUNDWORLDCREATOR_WORLDMANAGER_H
