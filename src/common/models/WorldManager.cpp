#include "WorldManager.h"

#include "../../io/Camera.h"
// #include "../../io/Window.h"

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
  const std::vector<PointLight>& point_lights,
  const std::vector<DirectedLight>& dir_lights,
  const std::vector<StaticObject>& static_objects)
    : scene_(scene),
  camera_(camera),
  player_(player),
  characters_(characters),
  weapons_(weapons),
  point_lights_(point_lights),
  dir_lights_(dir_lights),
  static_objects_(static_objects) {}

//TODO: Furthermore, running recursive std::function calls inside your hottest
// frame loop is going to eat your CPU budget
//TODO: so need to flatten to culled_nodes (not all scene nodes)
void WorldManager::PushFrustumCulled(
  const std::vector<int>& objects,
  std::vector<std::vector<InstanceGpu>>& ssbo_data) {
  const Scene::SceneData& scene_data = scene_->scene_data_;
  std::function<void(const Scene::ModelNode*)> dfs =
    [&](const Scene::ModelNode* node) {
      InstanceGpu new_instance;
      new_instance.model = node->global_transform;
      //TODO: primitives 0.... but maybe all our primitives have same mat id..
      // so fix not for today
      new_instance.material_id = scene_data.meshes[node->mesh_index].primitives[0].material_id;
      ssbo_data[node->mesh_index].push_back(std::move(new_instance));
      for (const Scene::ModelNode* child : node->children) {
        dfs(child);
      }
  };
  for (int idx : objects) {
    const Scene::ModelNode* node = static_objects_[idx].object_;
    dfs(node);
  }
}

void WorldManager::Cull() {

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

  for (const auto& c : characters_) {
    final_render_data_.camera.object_animated.push_back(
      c->GetBody()->GetAnimatedRenderData());
  }
  for (const auto& w : weapons_) {
    final_render_data_.camera.object_animated.push_back(
      w->GetAnimatedRenderData());
  }

  final_render_data_.point_lights.clear();
  final_render_data_.point_lights.resize(active_point_lights_.size());
  final_render_data_.dir_lights.clear();
  final_render_data_.dir_lights.resize(active_dir_lights_.size());

  auto frustum_camera = camera_->GetFrustum();

  /// HERE packed by object ids withing the current zone (not packed by meshes)
  //TODO: (dbg) obj->color.ToVec4(),
  for (int idx = 0; idx < static_objects_.size(); ++idx) {
    const auto& obj = static_objects_[idx];
    if (true || frustum_camera.Overlaps(obj.object_->bounds)) {
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
