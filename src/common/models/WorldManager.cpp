#include "WorldManager.h"

#include <iostream>

#include "../../io/Camera.h"
// #include "../../io/Window.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "DirectedLight.h"
#include "EnemyController.h"
#include "PlayerController.h"
#include "PointLight.h"
#include "Scene.h"
#include "StaticObject.h"
#include "Weapon.h"

WorldManager::WorldManager(
    const Scene* scene, const Camera* camera,
    const std::unique_ptr<PlayerController>& player,
    const std::vector<std::unique_ptr<EnemyController>>& characters,
    const std::vector<std::unique_ptr<Weapon>>& weapons,
    const std::vector<DirectedLight>& dir_lights)
    : scene_(scene),
      camera_(camera),
      player_(player),
      characters_(characters),
      weapons_(weapons),
      dir_lights_(dir_lights),
      random_floats_(0.0, 1.0),
      generator_() {}

// TODO: Furthermore, running recursive std::function calls inside your hottest
//  frame loop is going to eat your CPU budget
// TODO: so need to flatten to culled_nodes (not all scene nodes)
void WorldManager::PushFrustumCulled(
    const SceneNode* node, std::vector<std::vector<InstanceGpu>>& ssbo_data,
    bool debug) {
  const Scene::SceneData& scene_data = scene_->scene_data_;
  InstanceGpu new_instance;
  float dispersion = 100.0f;
  if (debug) {
    dispersion = 255.0f;
    JPH::Vec3 box_size = node->local_bounds.GetExtent();
    auto scale_matrix = JPH::Mat44::sScale(box_size);
    new_instance.model = node->global_transform * scale_matrix;
  } else {
    new_instance.model = node->global_transform;
  }
  // TODO: primitives 0.... but maybe all our primitives have same mat id..
  //  so fix not for today
  new_instance.material_id =
      node->mesh->primitives_lods[0][0].material_id;

  float rest = 255.0f - dispersion;
  auto GetRandomValue = [&]() -> float {
    return rest + std::clamp(0.0f, 255.0f, float(node->node_id)) * dispersion / 255.0f;
    // return rest + dispersion * random_floats_(generator_);
  };
  new_instance.color = JPH::Vec4(GetRandomValue(), GetRandomValue(), GetRandomValue(), 255.0f);
  auto mesh_type = GetAssetDefaultProperties(node->mesh->name).type;
  if (mesh_type == AssetProperties::Type::Wall || mesh_type == AssetProperties::Type::Tile) {
    if (new_instance.material_id == static_cast<int>(MaterialIndex::Clay)) {
      new_instance.use_triplanar = 40;
    } else {
      new_instance.use_triplanar = 40;
      // new_instance.use_triplanar = 15;
    }
  }
  ssbo_data[node->mesh->id].push_back(std::move(new_instance));
  for (const SceneNode* child : node->children) {
    // PushFrustumCulled(child, ssbo_data, debug);
  }
}

void WorldManager::PushFrustumCulled(
    const std::vector<const SceneNode*>& objects,
    std::vector<std::vector<InstanceGpu>>& ssbo_data,
    bool debug) {
  const auto& zones = scene_->scene_data_.zones;
  for (auto node : objects) {
    PushFrustumCulled(node, ssbo_data, debug);
  }
}

// specifically for character and weapons (they're not included by zones)
bool WorldManager::IsZoneCulled(const std::set<int>& zones, JPH::Vec3 pos) {
  auto character_zone = FindNearestZone(pos);
  auto it =
      std::find(active_zones_.begin(), active_zones_.end(), character_zone);
  return it == active_zones_.end();
}

JPH::AABox GetDefaultBounds() {
  return JPH::AABox(JPH::Vec3::sReplicate(-1.0f), JPH::Vec3::sReplicate(1.0f));
}

int GetLodByDist(JPH::Vec3 camera_pos, JPH::Vec3 obj_pos) {
  int lod = 2;
  auto dist_to_camera = (camera_pos - obj_pos).Length();
  if (dist_to_camera < 4.0f) {
    lod = 0;
  } else if (dist_to_camera < 8.0f) {
    lod = 1;
  }
  return lod;
}

void WorldManager::CullAnimatedObject(
    const std::set<int>& zones,
    const Frustum& camera_frustum, JPH::Vec3 camera_pos,
    const std::vector<PointLight*>& zone_point_lights, JPH::Vec3 pos,
    AnimatedRenderData render_data, bool apply_directed_lights,
    bool is_player) {
  if (IsZoneCulled(zones, pos)) {
    return;
  }
  auto bounds = GetDefaultBounds();
  bounds.Translate(pos);
  // we render player with separate call in Geometry pass
  if (!is_player && camera_frustum.Overlaps(bounds)) {
    render_data.lod = GetLodByDist(camera_pos, pos);
    final_render_data_.camera.object_animated.push_back(render_data);
  }
  render_data.lod = 2;  // min lod for shadows
  if (apply_directed_lights) {
    for (int light_id = 0; light_id < dir_lights_.size(); ++light_id) {
      const auto& light = dir_lights_[light_id];
      if (light.frustum.Overlaps(bounds)) {
        final_render_data_.dir_lights[light_id].object_animated.push_back(
            render_data);
      }
    }
  }
  for (int light_id = 0; light_id < zone_point_lights.size(); ++light_id) {
    const auto& light = zone_point_lights[light_id];
    auto light_pos = light->GetSceneNode()->global_bounds.GetCenter();
    for (int face = 0; face < 6; ++face) {
      if (!light->frustum_[face].is_visible) {
        continue;
      }
      if (light->frustum_[face].frustum.Overlaps(bounds, light_pos,
                                                 light->radius_)) {
        final_render_data_.point_lights[light_id]
            .object_animated[face]
            .push_back(render_data);
      }
    }
  }
}

struct FaceTarget {
  glm::vec3 dir;
  glm::vec3 up;
};
// TODO: same declared in Renderer.cpp, so need to unify em

// Modern OpenGL cubemap face sampling order (+X, -X, +Y, -Y, +Z, -Z)
static const FaceTarget kCubeFaces[6] = {
    {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},   // +X
    {glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},  // -X
    {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},    // +Y
    {glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},  // -Y
    {glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},   // +Z
    {glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)}   // -Z
};

void UpdateLightFrustums(const Frustum& camera_frustum, std::unique_ptr<PointLight>& l) {
  // int total_faces_rendered = 0;
  for (int i = 0; i < std::size(kCubeFaces); ++i) {
    auto face = kCubeFaces[i];
    auto pos = l->GetSceneNode()->global_bounds.GetCenter();
    auto fov = JPH::DegreesToRadians(90.0f);
    float near = 0.01f;
    l->frustum_[i].frustum =
        Frustum(pos, ToJph(face.dir), ToJph(face.up), fov, fov, near);
    l->frustum_[i].is_visible =
        camera_frustum.OverlapsLightFace(pos, l->radius_, i);
    // if (l.frustum_[i].is_visible) ++total_faces_rendered;
  }
  // std::cout << total_faces_rendered << std::endl;
}

void WorldManager::Cull(bool prebake_ibl_pass, bool skip_portals,
    bool render_animated, bool debug, int total_objects) {
  final_render_data_.render_player = !prebake_ibl_pass;
  random_floats_ = std::uniform_real_distribution<float>(0.0, 1.0);
  generator_ = std::default_random_engine();

  auto camera_frustum = camera_->GetFrustum();
  for (auto& l :
       scene_->scene_data_.zones[cur_zone_id_]->point_lights_) {
    UpdateLightFrustums(camera_frustum, l);
  }

  const auto& zones = scene_->scene_data_.zones;

  bool apply_directed_light = false;
  std::vector<PointLight*> point_lights;

  auto active_render_zones = GetActiveRenderZones();

  for (int zone_id : active_render_zones) {
    auto* zone = zones[zone_id];
    if (zone->properties.outside) {
      apply_directed_light = true;
    }
    if (prebake_ibl_pass && zone_id != cur_zone_id_) {
      continue;
    }
    for (auto& l : zone->point_lights_) {
      point_lights.push_back(l.get());
    }
  }

  final_render_data_.directed_lights_on = apply_directed_light;

  active_point_lights_ =
      std::vector<std::array<std::vector<const SceneNode*>, 6>>(
          point_lights.size());
  active_dir_lights_ =
      std::vector<std::vector<const SceneNode*>>(dir_lights_.size());
  active_camera_transparent = std::vector<const SceneNode*>();
  active_camera_lod0 = std::vector<const SceneNode*>();
  active_camera_lod1 = std::vector<const SceneNode*>();
  active_camera_lod2 = std::vector<const SceneNode*>();
  final_render_data_.ssbo_data.clear();
  final_render_data_.point_lights_fake.clear();

  final_render_data_.camera.objects_transparent.clear();
  final_render_data_.camera.objects_lod0.clear();
  final_render_data_.camera.objects_lod1.clear();
  final_render_data_.camera.objects_lod2.clear();
  final_render_data_.camera.object_animated.clear();

  final_render_data_.point_lights.clear();
  final_render_data_.point_lights.resize(active_point_lights_.size());
  final_render_data_.dir_lights.clear();
  final_render_data_.dir_lights.resize(active_dir_lights_.size());

  auto camera_pos = ToJph(camera_->GetPosition());

  auto PackSSBO = [&](const std::vector<std::vector<InstanceGpu>>& source_data,
                    std::vector<SsboOffsetObject>& target_offsets) {
    for (int mesh_id = 0; mesh_id < source_data.size(); ++mesh_id) {
      const auto& data = source_data[mesh_id];
      if (!data.empty()) {
        SsboOffsetObject new_offset;
        new_offset.instance_offset = final_render_data_.ssbo_data.size();
        new_offset.instances_num = data.size();
        new_offset.mesh_id = mesh_id;
        target_offsets.push_back(new_offset);

        final_render_data_.ssbo_data.insert(
            final_render_data_.ssbo_data.end(),
            data.begin(), data.end()
        );
      }
    }
  };

  auto CheckStaticObjects = [&] (const std::vector<std::unique_ptr<StaticObject>>& static_objects) {
    for (auto& obj : static_objects) {
      SceneNode* obj_scene_node = obj->GetSceneNode();
      if (skip_portals) {
        auto mesh_type =
          GetAssetDefaultProperties(obj_scene_node->mesh->name).type;
        if (mesh_type == AssetProperties::Type::HingeMoving) {
          continue;
        }
      }
      if (camera_frustum.Overlaps(obj_scene_node->global_bounds)) {
        if (--total_objects < 0) {
          continue;
        }
        //TODO: closes point of aabb instead
        auto lod = GetLodByDist(camera_pos, obj_scene_node->global_bounds.GetCenter());
        if (lod == 0) {
          active_camera_lod0.push_back(obj_scene_node);
        } else if (lod == 1) {
          active_camera_lod1.push_back(obj_scene_node);
        } else {
          active_camera_lod2.push_back(obj_scene_node);
        }
      }
    }
  };

  auto CheckTransparentObjects = [&] (const std::vector<std::unique_ptr<StaticObject>>& static_objects) {
    for (auto& obj : static_objects) {
      SceneNode* obj_scene_node = obj->GetSceneNode();
      if (skip_portals) {
        auto mesh_type =
          GetAssetDefaultProperties(obj_scene_node->mesh->name).type;
        if (mesh_type == AssetProperties::Type::HingeMoving) {
          continue;
        }
      }
      if (camera_frustum.Overlaps(obj_scene_node->global_bounds)) {
        if (--total_objects < 0) {
          continue;
        }
        active_camera_transparent.push_back(obj_scene_node);
      }
    }
  };

  auto CheckStaticObjectsLight = [&] (const std::vector<std::unique_ptr<StaticObject>>& static_objects) {
    for (auto& obj : static_objects) {
      SceneNode* obj_scene_node = obj->GetSceneNode();
      if (skip_portals) {
        auto mesh_type =
          GetAssetDefaultProperties(obj_scene_node->mesh->name).type;
        if (mesh_type == AssetProperties::Type::HingeMoving) {
          continue;
        }
      }
      if (apply_directed_light) {
        for (int light_id = 0; light_id < dir_lights_.size(); ++light_id) {
          const auto& light = dir_lights_[light_id];
          if (light.frustum.Overlaps(obj_scene_node->global_bounds)) {
            active_dir_lights_[light_id].push_back(obj_scene_node);
          }
        }
      }
      for (int light_id = 0; light_id < point_lights.size(); ++light_id) {
        const auto& light = point_lights[light_id];
        auto light_pos = light->GetSceneNode()->global_bounds.GetCenter();
        for (int face = 0; face < 6; ++face) {
          if (!light->frustum_[face].is_visible) {
            continue;
          }
          if (light->frustum_[face].frustum.Overlaps(
                  obj_scene_node->global_bounds, light_pos, light->radius_)) {
            active_point_lights_[light_id][face].push_back(obj_scene_node);
                  }
        }
      }
    }
  };

  for (int zone_id : active_render_zones) {
    if (prebake_ibl_pass && zone_id != cur_zone_id_) {
      continue;
    }
    const auto* zone = zones[zone_id];

    if (render_animated) {
      for (const auto& c : characters_) {
        CullAnimatedObject(active_render_zones, camera_frustum, ToJph(camera_->GetPosition()),
                           point_lights, c->GetBody()->GetPosition(),
                           c->GetBody()->GetAnimatedRenderData(),
                           apply_directed_light, false);
      }
      CullAnimatedObject(active_render_zones, camera_frustum, ToJph(camera_->GetPosition()),
                         point_lights, player_->GetBody()->GetPosition(),
                         player_->GetBody()->GetAnimatedRenderData(),
                         apply_directed_light, true);
      for (const auto& w : weapons_) {
        CullAnimatedObject(active_render_zones, camera_frustum, ToJph(camera_->GetPosition()),
                           point_lights, w->GetPosition(),
                           w->GetAnimatedRenderData(),
                           apply_directed_light, false);
      }
    }

    for (auto& obj : zone->point_lights_fake_) {
      if (camera_frustum.Overlaps(obj->GetSceneNode()->global_bounds)) {
        final_render_data_.point_lights_fake.push_back(obj->GetSceneNode());
      }
    }

    CheckStaticObjects(zone->static_objects_);
    CheckStaticObjectsLight(zone->static_objects_);
    CheckTransparentObjects(zone->transparent_objects_);
    CheckStaticObjectsLight(zone->transparent_objects_);
  }

  /// linealization (we have FrustumCulledObjects, now
  /// need to generate only solid InstanceGpu block, what now is BY MESH,
  /// so need to go through the hierarchy
  /// --- SO HERE we packing instnce data (ssbo) camera - plight - dlight
  const Scene::SceneData& scene_data = scene_->scene_data_;
  int meshes_num = scene_data.meshes.size();
  auto ssbo_data_cam_transparent = std::vector<std::vector<InstanceGpu>>(meshes_num);
  auto ssbo_data_cam_lod0 = std::vector<std::vector<InstanceGpu>>(meshes_num);
  auto ssbo_data_cam_lod1 = std::vector<std::vector<InstanceGpu>>(meshes_num);
  auto ssbo_data_cam_lod2 = std::vector<std::vector<InstanceGpu>>(meshes_num);
  auto ssbo_data_pl =
      std::vector<std::array<std::vector<std::vector<InstanceGpu>>, 6>>();
  auto ssbo_data_dl = std::vector<std::vector<std::vector<InstanceGpu>>>();
  PushFrustumCulled(active_camera_transparent, ssbo_data_cam_transparent, debug);
  PushFrustumCulled(active_camera_lod0, ssbo_data_cam_lod0, debug);
  PushFrustumCulled(active_camera_lod1, ssbo_data_cam_lod1, debug);
  PushFrustumCulled(active_camera_lod2, ssbo_data_cam_lod2, debug);
  // std::cout << zone->portals.size() << std::endl;

  /// --- DECALS ---
  const SceneMesh& decal_mesh = scene_->scene_data_.meshes[scene_->mesh_shoot_hole_id];
  for (const auto& d : *decals_) {
    auto decal_pos = d.transform.GetTranslation();
    if (camera_frustum.Overlaps(JPH::AABox(decal_pos, 0.1f))) {
      InstanceGpu new_instance;
      new_instance.model = d.transform;
      new_instance.material_id =
        decal_mesh.primitives_lods[0][0].material_id;
      // auto lod = GetLodByDist(camera_pos, decal_pos);
      ssbo_data_cam_transparent[decal_mesh.id].push_back(std::move(new_instance));
    }
  }

  for (int i = 0; i < active_point_lights_.size(); ++i) {
    ssbo_data_pl.push_back(
        std::array<std::vector<std::vector<InstanceGpu>>, 6>());
    for (int face = 0; face < 6; ++face) {
      ssbo_data_pl.back()[face] =
          std::vector<std::vector<InstanceGpu>>(meshes_num);
      PushFrustumCulled(active_point_lights_[i][face],
                        ssbo_data_pl.back()[face], debug);
    }
  }
  for (int i = 0; i < active_dir_lights_.size(); ++i) {
    ssbo_data_dl.push_back(std::vector<std::vector<InstanceGpu>>(meshes_num));
    PushFrustumCulled(active_dir_lights_[i], ssbo_data_dl.back(), debug);
  }

  final_render_data_.ssbo_data.clear();
  PackSSBO(ssbo_data_cam_transparent, final_render_data_.camera.objects_transparent);
  PackSSBO(ssbo_data_cam_lod0, final_render_data_.camera.objects_lod0);
  PackSSBO(ssbo_data_cam_lod1, final_render_data_.camera.objects_lod1);
  PackSSBO(ssbo_data_cam_lod2, final_render_data_.camera.objects_lod2);

  for (int i = 0; i < active_point_lights_.size(); ++i) {
    auto& l_out = final_render_data_.point_lights[i];
    l_out.source = point_lights[i];
    for (int face = 0; face < 6; ++face) {
      PackSSBO(ssbo_data_pl[i][face], l_out.objects[face]);
    }
  }

  for (int i = 0; i < active_dir_lights_.size(); ++i) {
    auto& l_out = final_render_data_.dir_lights[i];
    l_out.source = &dir_lights_[i];
    PackSSBO(ssbo_data_dl[i], l_out.objects);
  }
  final_render_data_.active_zones = active_render_zones;
}

int WorldManager::FindNearestZone(JPH::Vec3 player_pos) {
  const auto& zones = scene_->scene_data_.zones;

  int nearest_index = 0;
  float min_sq_dist = FLT_MAX;

  for (int i = 0; i < zones.size(); ++i) {
    // GetSqDistanceTo returns 0.0f if the point is inside the AABB
    float sq_dist = zones[i]->scene_node->global_bounds.GetSqDistanceTo(player_pos);

    // if (sq_dist < min_sq_dist) {
    //   min_sq_dist = sq_dist;
    //   nearest_index = i;
    // }

    // Early exit: We are perfectly inside a zone
    if (sq_dist == 0.0f) {
      return i;
    }
  }
  return nearest_index;
}

// Call this every frame in your Update loop

void PrintActiveZones(const std::set<int>& zones) {
  std::cout << "Active zones: ";
  for (auto id : zones) {
    std::cout << id << ' ';
  }
  std::cout << std::endl;
}

std::set<int> WorldManager::GetActiveRenderZones() {
  std::set<int> active_zones;
  auto camera_frustum = camera_->GetFrustum();
  const auto& zones = scene_->scene_data_.zones;
  auto dfs = [&](auto& self, int zone_id) {
    if (zone_id == -1 || active_zones.contains(zone_id)) {
      return;
    }
    active_zones.insert(zone_id);
    const auto& portals = zones[zone_id]->portals;
    for (ScenePortal* p : portals) {
      if (camera_frustum.Overlaps(p->scene_node->global_bounds.Scaled(
        JPH::Vec3::sReplicate(1.2f)))) {
        self(self, p->zone_1);
        self(self, p->zone_2);
      }
    }
  };
  dfs(dfs, cur_zone_id_);
  return active_zones;
}

std::set<int> WorldManager::GetActivePhysicsZones() {
  int depth = 1;
  std::set<int> active_zones;
  const auto& zones = scene_->scene_data_.zones;
  auto dfs = [&](auto& self, int zone_id, int depth) {
    if (depth < 0) {
      return;
    }
    if (zone_id == -1 || active_zones.contains(zone_id)) {
      return;
    }
    active_zones.insert(zone_id);
    const auto& portals = zones[zone_id]->portals;
    for (ScenePortal* p : portals) {
      self(self, p->zone_1, depth - 1);
      self(self, p->zone_2, depth - 1);
    }
  };
  dfs(dfs, cur_zone_id_, depth);
  return active_zones;
}

bool WorldManager::UpdatePlayerZone(JPH::Vec3 player_pos,
                                    JPH::BodyInterface* body_interface) {
  const auto& zones = scene_->scene_data_.zones;
  if (cur_zone_id_ != -1 &&
      zones[cur_zone_id_]->scene_node->global_bounds.Scaled(
      JPH::Vec3::sReplicate(1.01f)).Contains(player_pos)) {
    return false;
  }
  auto new_cur_zone_id_ = FindNearestZone(player_pos);
  if (new_cur_zone_id_ == cur_zone_id_) {
    return false;
  }
  cur_zone_id_ = new_cur_zone_id_;
  auto prev_active_zones = active_zones_;
  auto next_active_zones = GetActivePhysicsZones();

  std::set<int> zones_to_deactivate;
  std::set<int> zones_to_activate;

  std::set_difference(
      prev_active_zones.begin(), prev_active_zones.end(),
      next_active_zones.begin(), next_active_zones.end(),
      std::inserter(zones_to_deactivate, zones_to_deactivate.begin()));

  std::set_difference(
      next_active_zones.begin(), next_active_zones.end(),
      prev_active_zones.begin(), prev_active_zones.end(),
      std::inserter(zones_to_activate, zones_to_activate.begin()));

  for (auto zone_id : zones_to_deactivate) {
    DeactivateZone(zone_id, body_interface);
  }

  for (auto zone_id : zones_to_activate) {
    ActivateZone(zone_id, body_interface);
  }

  active_zones_ = std::move(next_active_zones);
  PrintActiveZones(active_zones_);
  return true;
}

void WorldManager::ActivateZone(int zone_index,
                                JPH::BodyInterface* body_interface) {
  auto ActivateJphBodies = [&](auto& self, SceneNode* node) -> void {
    JPH::BodyID body_id = node->body_id;
    if (!body_id.IsInvalid() && node->can_be_activated) {
      body_interface->ActivateBody(body_id);
    }
    for (auto* child : node->children) {
      self(self, child);
    }
  };
  const auto& zones = scene_->scene_data_.zones;
  const SceneZone* zone = zones[zone_index];
  for (SceneNode* obj : zone->object_nodes) {
    ActivateJphBodies(ActivateJphBodies, obj);
  }
}

void WorldManager::DeactivateZone(int zone_index,
                                  JPH::BodyInterface* body_interface) {
  auto DeactivateJphBodies = [&](auto& self, const SceneNode* node) -> void {
    JPH::BodyID body_id = node->body_id;
    if (!body_id.IsInvalid() && node->can_be_activated) {
      body_interface->DeactivateBody(body_id);
    }
    for (const auto* child : node->children) {
      self(self, child);
    }
  };
  const auto& zones = scene_->scene_data_.zones;
  const SceneZone* zone = zones[zone_index];
  for (const SceneNode* obj : zone->object_nodes) {
    DeactivateJphBodies(DeactivateJphBodies, obj);
  }
}

const SceneZone* WorldManager::GetCurrentZone() {
  const auto& zones = scene_->scene_data_.zones;
  return zones[cur_zone_id_];
}

