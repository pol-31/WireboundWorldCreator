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

// specifically for character and weapons (they're not included by zones)
bool WorldManager::IsZoneCulled(JPH::Vec3 pos) {
  auto character_zone = FindNearestZone(pos);
  auto it =
    std::find(active_zones_.begin(), active_zones_.end(), character_zone);
  return it == active_zones_.end();
}

JPH::AABox GetDefaultBounds() {
  return JPH::AABox(
      JPH::Vec3::sReplicate(-1.0f),
      JPH::Vec3::sReplicate(1.0f));
}

void WorldManager::CullAnimatedObject(
    const Frustum& frustum_camera,
    const std::vector<PointLight>& zone_point_lights,
    JPH::Vec3 pos,
    const AnimatedRenderData& render_data) {
  if (IsZoneCulled(pos)) {
    return;
  }
  auto bounds = GetDefaultBounds();
  bounds.Translate(pos);
  if (frustum_camera.Overlaps(bounds)) {
    final_render_data_.camera.object_animated.push_back(render_data);
  }
  for (int light_id = 0; light_id < dir_lights_.size(); ++light_id) {
    const auto& light = dir_lights_[light_id];
    if (light.frustum.Overlaps(bounds)) {
      final_render_data_.dir_lights[light_id].object_animated.push_back(render_data);
    }
  }
  for (int light_id = 0; light_id < zone_point_lights.size(); ++light_id) {
    const auto& light = zone_point_lights[light_id];
    auto light_pos = light.object_->global_bounds.GetCenter();
    for (int face = 0; face < 6; ++face) {
      if (!light.frustum_[face].is_visible) {
        continue;
      }
      if (light.frustum_[face].frustum.Overlaps(bounds, light_pos, light.radius_)) {
        final_render_data_.point_lights[light_id].object_animated[face].push_back(render_data);
      }
    }
  }
}

struct FaceTarget {
  glm::vec3 dir;
  glm::vec3 up;
};
//TODO: same declared in Renderer.cpp, so need to unify em

// Modern OpenGL cubemap face sampling order (+X, -X, +Y, -Y, +Z, -Z)
static const FaceTarget kCubeFaces[6] = {
  { glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f) }, // +X
  { glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f) }, // -X
  { glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f) }, // +Y
  { glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f) }, // -Y
  { glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f) }, // +Z
  { glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f) }  // -Z
};

void UpdateLightFrustums(const Frustum& frustum_camera, PointLight& l) {
  //int total_faces_rendered = 0;
  for (int i = 0; i < std::size(kCubeFaces); ++i) {
    auto face = kCubeFaces[i];
    auto pos = l.object_->global_bounds.GetCenter();
    auto fov = JPH::DegreesToRadians(90.0f);
    float near = 0.01f;
    l.frustum_[i].frustum = Frustum(pos, ToJph(face.dir), ToJph(face.up),
      fov, fov, near);
    l.frustum_[i].is_visible = frustum_camera.OverlapsLightFace(pos, l.radius_, i);
    //if (l.frustum_[i].is_visible) ++total_faces_rendered;
  }
  //std::cout << total_faces_rendered << std::endl;
}

void WorldManager::Cull() {
  //TODO: check active zones as well;
  auto frustum_camera = camera_->GetFrustum();
  for (auto& l : scene_->scene_data_.tiles[0]->zones[cur_zone_id_]->point_lights_) {
    UpdateLightFrustums(frustum_camera, l);
  }

  //TODO: check active zones as well;
  const auto& zones = scene_->scene_data_.tiles[0]->zones;
  const Scene::Zone* zone = zones[cur_zone_id_];
  const auto& point_lights_ = zone->point_lights_;


  const auto& static_objects_ = zone->static_objects_;

  active_point_lights_ = std::vector<std::array<std::vector<int>, 6>>(point_lights_.size());
  active_dir_lights_ = std::vector<std::vector<int>>(dir_lights_.size());
  active_camera_ = std::vector<int>();
  final_render_data_.ssbo_data.clear();

  final_render_data_.camera.objects.clear();
  final_render_data_.camera.object_animated.clear();

  final_render_data_.point_lights.clear();
  final_render_data_.point_lights.resize(active_point_lights_.size());
  final_render_data_.dir_lights.clear();
  final_render_data_.dir_lights.resize(active_dir_lights_.size());


  for (const auto& c : characters_) {
    CullAnimatedObject(frustum_camera, point_lights_, c->GetBody()->GetPosition(),
      c->GetBody()->GetAnimatedRenderData());
  }
  for (const auto& w : weapons_) {
    CullAnimatedObject(frustum_camera, point_lights_, w->GetPosition(),
      w->GetAnimatedRenderData());
  }

  /// HERE packed by object ids withing the current zone (not packed by meshes)
  //TODO: (dbg) obj->color.ToVec4(),
  for (int idx = 0; idx < static_objects_.size(); ++idx) {
    const auto& obj = static_objects_[idx];
    if (frustum_camera.Overlaps(obj.object_->global_bounds)) {
      active_camera_.push_back(idx);
    }
    for (int light_id = 0; light_id < dir_lights_.size(); ++light_id) {
      const auto& light = dir_lights_[light_id];
      if (light.frustum.Overlaps(obj.object_->global_bounds)) {
        active_dir_lights_[light_id].push_back(idx);
      }
    }
    for (int light_id = 0; light_id < point_lights_.size(); ++light_id) {
      const auto& light = point_lights_[light_id];
      auto light_pos = light.object_->global_bounds.GetCenter();
      for (int face = 0; face < 6; ++face) {
        if (!light.frustum_[face].is_visible) {
          continue;
        }
        if (light.frustum_[face].frustum.Overlaps(obj.object_->global_bounds, light_pos, light.radius_)) {
          active_point_lights_[light_id][face].push_back(idx);
        }
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
  auto ssbo_data_pl = std::vector<std::array<std::vector<std::vector<InstanceGpu>>, 6>>();
  auto ssbo_data_dl = std::vector<std::vector<std::vector<InstanceGpu>>>();
  PushFrustumCulled(active_camera_, ssbo_data_cam);
  for (int i = 0; i < active_point_lights_.size(); ++i) {
      ssbo_data_pl.push_back(std::array<std::vector<std::vector<InstanceGpu>>, 6>());
    for (int face = 0; face < 6; ++face) {
      ssbo_data_pl.back()[face] = std::vector<std::vector<InstanceGpu>>(meshes_num);
      PushFrustumCulled(active_point_lights_[i][face], ssbo_data_pl.back()[face]);
    }
  }
  for (int i = 0; i < active_dir_lights_.size(); ++i) {
    ssbo_data_dl.push_back(std::vector<std::vector<InstanceGpu>>(meshes_num));
    PushFrustumCulled(active_dir_lights_[i], ssbo_data_dl.back());
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
    l_out.source = &point_lights_[i];
    for (int face = 0; face < 6; ++face) {
      const auto& face_data = ssbo_data_pl[i][face];
      for (int mesh_id = 0; mesh_id < face_data.size(); ++mesh_id) {
        auto& data = face_data[mesh_id]; // here from ssbo_data_cam
        if (!data.empty()) {
          SsboOffsetObject new_offset;
          new_offset.instance_offset = final_render_data_.ssbo_data.size();
          new_offset.instances_num = data.size();
          new_offset.mesh_id = mesh_id;
          l_out.objects[face].push_back(new_offset);
          final_render_data_.ssbo_data.insert(final_render_data_.ssbo_data.end(),
            data.begin(), data.end());
        }
      }
    }
  }
  for (int i = 0; i < active_dir_lights_.size(); ++i) {
    auto& l_out = final_render_data_.dir_lights[i];
    l_out.source = &dir_lights_[i];
    for (int mesh_id = 0; mesh_id < ssbo_data_dl[i].size(); ++mesh_id) {
      auto& data = ssbo_data_dl[i][mesh_id]; // herer from ssbo_data_cam
      if (!data.empty()) {
        SsboOffsetObject new_offset;
        new_offset.instance_offset = final_render_data_.ssbo_data.size();
        new_offset.instances_num = data.size();
        new_offset.mesh_id = mesh_id;
        l_out.objects.push_back(new_offset);
        final_render_data_.ssbo_data.insert(final_render_data_.ssbo_data.end(),
          data.begin(), data.end());
      }
    }
  }
  final_render_data_.active_zones = active_zones_;
}

int WorldManager::FindNearestZone(JPH::Vec3 player_pos) {
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
    cur_zone_id_ = FindNearestZone(player_pos);
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
    new_zone_index = FindNearestZone(player_pos);
  }

  // 5. Trigger the state change
  if (new_zone_index != cur_zone_id_) {
    DeactivateZone(cur_zone_id_, body_interface);
    for (auto p : zones[cur_zone_id_]->portals) {
      auto near_zone_id = p->connected_zone_index_1;
      if (near_zone_id == cur_zone_id_) {
        near_zone_id = p->connected_zone_index_2;
      }
      if (std::find(active_zones_.begin(), active_zones_.end(), near_zone_id) != active_zones_.end()) {
        DeactivateZone(near_zone_id, body_interface);
      }
    }
    cur_zone_id_ = new_zone_index;
    ActivateZone(cur_zone_id_, body_interface);
    std::cout << "Player entered Zone: " << cur_zone_id_ << std::endl;
    for (auto p : zones[cur_zone_id_]->portals) {
      auto near_zone_id = p->connected_zone_index_1;
      if (near_zone_id == cur_zone_id_) {
        near_zone_id = p->connected_zone_index_2;
      }
      if (std::find(active_zones_.begin(), active_zones_.end(), near_zone_id) == active_zones_.end()) {
          ActivateZone(near_zone_id, body_interface);
      }
    }
  }
  std::cout << "Active zones: ";
  for (auto id : active_zones_) {
    std::cout << id << ' ';
  }
  std::cout << std::endl;
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
