#include "WorldManager.h"

#include <iostream>

#include "../../io/Camera.h"
// #include "../../io/Window.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "Scene.h"
#include "StaticObject.h"
#include "DirectedLight.h"
#include "PointLight.h"
#include "PlayerController.h"
#include "EnemyController.h"
#include "Weapon.h"

WorldManager::WorldManager(
  const Scene* scene,
  const Camera* camera,
  const std::unique_ptr<PlayerController>& player,
  const std::vector<std::unique_ptr<EnemyController>>& characters,
  const std::vector<std::unique_ptr<Weapon>>& weapons,
  const std::vector<DirectedLight>& dir_lights)
    : scene_(scene),
  camera_(camera),
  player_(player),
  characters_(characters),
  weapons_(weapons),
  dir_lights_(dir_lights) {}

//TODO: Furthermore, running recursive std::function calls inside your hottest
// frame loop is going to eat your CPU budget
//TODO: so need to flatten to culled_nodes (not all scene nodes)
void WorldManager::PushFrustumCulled(
  const std::vector<int>& objects,
  std::vector<std::vector<InstanceGpu>>& ssbo_data) {
  const Scene::SceneData& scene_data = scene_->scene_data_;
  std::function<void(const SceneNode*)> dfs =
    [&](const SceneNode* node) {
      InstanceGpu new_instance;
      new_instance.model = node->global_transform;
      //TODO: primitives 0.... but maybe all our primitives have same mat id..
      // so fix not for today
      new_instance.material_id = scene_data.meshes[node->mesh_index].primitives[0].material_id;
      ssbo_data[node->mesh_index].push_back(std::move(new_instance));
      for (const SceneNode* child : node->children) {
        dfs(child);
      }
  };

  const auto& zones = scene_->scene_data_.tiles[0]->zones;
  const Scene::Zone* zone = zones[cur_zone_id_];
  for (int idx : objects) {
    const SceneNode* node = zone->static_objects_[idx].object_;
    dfs(node);
  }
}

void WorldManager::Cull() {
  const auto& zones = scene_->scene_data_.tiles[0]->zones;
  const Scene::Zone* zone = zones[cur_zone_id_];
  const auto& point_lights_ = zone->point_lights_;
  const auto& static_objects_ = zone->static_objects_;

  // if (!cur_tile_) {
    // throw std::runtime_error("no zone set");
  // }
  // const auto& zone = zones_[player_zone_];
  active_point_lights_ = std::vector<FrustumCulledObjects>(point_lights_.size());
  active_dir_lights_ = std::vector<FrustumCulledObjects>(dir_lights_.size());
  active_camera_ = FrustumCulledObjects();
  final_render_data_.ssbo_data.clear();

  final_render_data_.camera.objects.clear();
  final_render_data_.camera.object_animated.clear();

  auto frustum_camera = camera_->GetFrustum();

  for (const auto& c : characters_) {
    auto translation = c->GetBody()->GetPosition();
    auto bounds = JPH::AABox(
      JPH::Vec3::sReplicate(-1.0f), JPH::Vec3::sReplicate(1.0f));
    bounds.Translate(translation);
    if (frustum_camera.Overlaps(bounds)) {
      auto render_data = c->GetBody()->GetAnimatedRenderData();
      final_render_data_.camera.object_animated.push_back(
        render_data);
    }
  }

  for (const auto& w : weapons_) {
    final_render_data_.camera.object_animated.push_back(
      w->GetAnimatedRenderData());
  }

  final_render_data_.point_lights.clear();
  final_render_data_.point_lights.resize(active_point_lights_.size());
  final_render_data_.dir_lights.clear();
  final_render_data_.dir_lights.resize(active_dir_lights_.size());


  /// HERE packed by object ids withing the current zone (not packed by meshes)
  //TODO: (dbg) obj->color.ToVec4(),
  for (int idx = 0; idx < static_objects_.size(); ++idx) {
    const auto& obj = static_objects_[idx];
    if (frustum_camera.Overlaps(obj.object_->bounds)) {
      active_camera_.objects.push_back(idx);
    }
    for (int light_id = 0; light_id < dir_lights_.size(); ++light_id) {
      const auto& light = dir_lights_[light_id];
      if (light.frustum.Overlaps(obj.object_->bounds)) {
        active_dir_lights_[light_id].objects.push_back(idx);
      }
    }
    for (int light_id = 0; light_id < point_lights_.size(); ++light_id) {
      const auto& light = point_lights_[light_id];
      auto jph_pos = light.object_->bounds.GetCenter();
      if (obj.object_->bounds.Overlaps(JPH::AABox(jph_pos, light.radius_))) {
        active_point_lights_[light_id].objects.push_back(idx);
      }
    }
  }

  /// linealization (we have FrustumCulledObjects, now
  /// need to generate only solid InstanceGpu block, what now is BY MESH,
  /// so need to go through the hierarchy
  /// --- SO HERE we packing instnce data (ssbo) camera - plight - dlight
  const Scene::SceneData& scene_data = scene_->scene_data_;
  int meshes_num = scene_data.meshes.size();
  auto ssbo_data_cam = std::vector<std::vector<InstanceGpu>>(meshes_num);
  auto ssbo_data_pl = std::vector<std::vector<std::vector<InstanceGpu>>>();
  auto ssbo_data_dl = std::vector<std::vector<std::vector<InstanceGpu>>>();
  PushFrustumCulled(active_camera_.objects, ssbo_data_cam);
  for (int i = 0; i < active_point_lights_.size(); ++i) {
    ssbo_data_pl.push_back(std::vector<std::vector<InstanceGpu>>(meshes_num));
    PushFrustumCulled(active_point_lights_[i].objects, ssbo_data_pl.back());
  }
  for (int i = 0; i < active_dir_lights_.size(); ++i) {
    ssbo_data_dl.push_back(std::vector<std::vector<InstanceGpu>>(meshes_num));
    PushFrustumCulled(active_dir_lights_[i].objects, ssbo_data_dl.back());
  }

  final_render_data_.ssbo_data.clear();
  for (int mesh_id = 0; mesh_id < ssbo_data_cam.size(); ++mesh_id) {
    auto& data = ssbo_data_cam[mesh_id];
    if (!data.empty()) {
      SsboOffsetObject new_offset;
      new_offset.instance_offset = final_render_data_.ssbo_data.size();
      new_offset.instances_num = data.size();
      new_offset.mesh_id = mesh_id;
      final_render_data_.camera.objects.push_back(new_offset);
      final_render_data_.ssbo_data.insert(final_render_data_.ssbo_data.end(),
        data.begin(), data.end());
    }
  }
  for (int i = 0; i < active_point_lights_.size(); ++i) {
    auto& l_out = final_render_data_.point_lights[i];
    for (int mesh_id = 0; mesh_id < ssbo_data_pl[i].size(); ++mesh_id) {
      auto& data = ssbo_data_cam[mesh_id];
      if (!data.empty()) {
        SsboOffsetObject new_offset;
        new_offset.instance_offset = final_render_data_.ssbo_data.size();
        new_offset.instances_num = data.size();
        l_out.objects.push_back(new_offset);
        l_out.source = &point_lights_[i];
        final_render_data_.ssbo_data.insert(final_render_data_.ssbo_data.end(),
          data.begin(), data.end());
      }
    }
  }
  for (int i = 0; i < active_dir_lights_.size(); ++i) {
    auto& l_out = final_render_data_.dir_lights[i];
    for (int mesh_id = 0; mesh_id < ssbo_data_dl[i].size(); ++mesh_id) {
      auto& data = ssbo_data_cam[mesh_id];
      if (!data.empty()) {
        SsboOffsetObject new_offset;
        new_offset.instance_offset = final_render_data_.ssbo_data.size();
        new_offset.instances_num = data.size();
        l_out.objects.push_back(new_offset);
        l_out.source = &dir_lights_[i];
        final_render_data_.ssbo_data.insert(final_render_data_.ssbo_data.end(),
          data.begin(), data.end());
      }
    }
  }
}

int WorldManager::FindNearestZone(JPH::Vec3 player_pos, JPH::BodyInterface* body_interface) {
    const auto& zones = scene_->scene_data_.tiles[0]->zones;

        int nearest_index = -1;
        float min_sq_dist = FLT_MAX;

        for (int i = 0; i < zones.size(); ++i) {
            // GetSqDistanceTo returns 0.0f if the point is inside the AABB
            float sq_dist = zones[i]->bounds.GetSqDistanceTo(player_pos);

            if (sq_dist < min_sq_dist) {
                min_sq_dist = sq_dist;
                nearest_index = i;
            }

            // Early exit: We are perfectly inside a zone
            if (sq_dist == 0.0f) {
                return i;
            }
        }
        return nearest_index;
    }

    // Call this every frame in your Update loop

void WorldManager::UpdatePlayerZone(JPH::Vec3 player_pos, JPH::BodyInterface* body_interface) {
    const auto& zones = scene_->scene_data_.tiles[0]->zones;
  cur_tile_id_ = 0;
  // 1. Initial Spawn State
  if (cur_zone_id_ == -1) {
    cur_zone_id_ = FindNearestZone(player_pos, body_interface);
    ActivateZone(cur_zone_id_, body_interface);
    return;
  }

  // 2. O(1) Fast Path: Are we still in the same zone?
  if (zones[cur_zone_id_]->bounds.Contains(player_pos)) {
    return; // Skip activation process entirely
  }

  // 3. O(K) Transition Path: We stepped out. Check connected zones via portals.
  int new_zone_index = -1;
  for (const Scene::Portal* portal : zones[cur_zone_id_]->portals) {
    if (zones[portal->connected_zone_index_1]->bounds.Contains(player_pos)) {
      new_zone_index = portal->connected_zone_index_1;
      break;
    }
    if (zones[portal->connected_zone_index_2]->bounds.Contains(player_pos)) {
      new_zone_index = portal->connected_zone_index_2;
      break;
    }
  }

  // 4. Fallback: We glitched out of bounds or teleported without telling the manager
  if (new_zone_index == -1) {
    new_zone_index = FindNearestZone(player_pos, body_interface);
  }

  // 5. Trigger the state change
  if (new_zone_index != cur_zone_id_) {
    DeactivateZone(cur_zone_id_, body_interface);
    cur_zone_id_ = new_zone_index;
    ActivateZone(cur_zone_id_, body_interface);
    std::cout << "Player entered Zone: " << cur_zone_id_ << std::endl;
  }
}

void WorldManager::ActivateZone(int zone_index, JPH::BodyInterface* body_interface) {
  const auto& zones = scene_->scene_data_.tiles[0]->zones;
  const Scene::Zone* zone = zones[zone_index];
  for (const SceneNode* obj : zone->object_nodes) {
    JPH::BodyID body_id = obj->body_id;
    if (!body_id.IsInvalid() && obj->can_be_activated) {
      body_interface->ActivateBody(body_id);
    }
  }
  active_zones_.push_back(zone_index);
}

void WorldManager::DeactivateZone(int zone_index, JPH::BodyInterface* body_interface) {
  const auto& zones = scene_->scene_data_.tiles[0]->zones;
  const Scene::Zone* zone = zones[zone_index];
  for (const SceneNode* obj : zone->object_nodes) {
    JPH::BodyID body_id = obj->body_id;
    if (!body_id.IsInvalid() && obj->can_be_activated) {
      body_interface->DeactivateBody(body_id);
    }
  }
  std::erase(active_zones_, zone_index);
}
