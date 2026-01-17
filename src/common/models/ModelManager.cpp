#include "ModelManager.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

#include "../../core/TileRenderer.h"
#include "../../core/Ui.h"
#include "../../io/Camera.h"

ModelManager::ModelManager(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      mdl_loader_(ui_shared_resources, loader_),
      animator_(loader_),
      player_fpv_(ui_shared_resources),
      player_human_(ui_shared_resources),
      player_(&player_human_) {

  glCreateBuffers(1, &player_ubo_);
  glNamedBufferStorage(player_ubo_, sizeof(glm::vec3), nullptr, GL_DYNAMIC_STORAGE_BIT);
  glBindBufferBase(GL_UNIFORM_BUFFER, 4, player_ubo_);
  //TODO: delete ubo

  loader_.SetImageLoader(LoadImageData, nullptr);
  // auto mdl_musca =
  // mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Musca.gltf", 1);
  // auto mdl_snowman =
  // mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Horse.gltf", 2);

  animator_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\EnemyFixed.gltf");
  auto mdl_tree =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Tree.gltf", 1);
  auto mdl_human =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\EnemyFixed.gltf", 2);
  auto mdl_fpv = mdl_loader_.Load(
      "C:\\Users\\Pavlo\\Desktop\\assets\\AnimatedFpv3.gltf", 3);
  // auto mdl_tree =
  // mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\TreeNew.gltf", 3);
  // auto mdl_bush =
  // mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Bush.gltf", 4);
  // auto mdl_tall_grass =
  // mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\TallGrass.gltf", 5);
  // auto mdl_undergrowth = mdl_loader_.Load(
  // "C:\\Users\\Pavlo\\Desktop\\assets\\Undergrowth.gltf", 6);

  player_human_.SetModelData(mdl_human, &animator_, &attack_queue_);
  player_fpv_.SetModelData(mdl_fpv, &animator_, &attack_queue_);
  int creatures_num = 3;
  for (int i = 0; i < creatures_num; ++i) {
    creatures_.emplace_back();
    creatures_[i].SetModelData(mdl_human, &animator_, &attack_queue_);
    creatures_[i].SetPosition(glm::vec3(i + 1, 0.0f, i));
  }
  tree_.SetModelData(mdl_tree);
  bush_.SetModelData(mdl_tree);
  tall_grass_.SetModelData(mdl_tree);
  undergrowth_.SetModelData(mdl_tree);
  map_point_.SetModel(mdl_tree);
}

void ModelManager::Render() {
  for (auto& c : creatures_) {
    c.Render(ui_shared_resources_);
  }
  tree_.Render(ui_shared_resources_);
  bush_.Render(ui_shared_resources_);
  tall_grass_.Render(ui_shared_resources_);
  undergrowth_.Render(ui_shared_resources_);

  //  for (auto& o : obstacles_) {
  //    o.Render(ui_shared_resources_);
  //  }
  player_->Render(ui_shared_resources_);
}

void ModelManager::RenderPlacement() {
  tree_.Render(ui_shared_resources_);
  bush_.Render(ui_shared_resources_);
  tall_grass_.Render(ui_shared_resources_);
  undergrowth_.Render(ui_shared_resources_);
}

void ModelManager::RenderOnMap(UiDynamicSprite* sp_player,
                               UiDynamicSprite* sp_enemy,
                               UiDynamicSprite* sp_friend,
                               UiDynamicSprite* sp_neutral,
                               UiDynamicSprite* sp_obstacle) {
  ui_shared_resources_.shader_sp_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();

  auto map_scale =
      ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  glm::mat4 model;
  auto view =
      ui_shared_resources_.glfw_context_.camera->GetViewMatrix(map_scale);
  auto projection = ui_shared_resources_.glfw_context_.camera->GetProjMatrix();
  for (auto& c : creatures_) {
    model = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
    auto mvp = projection * view * model;
    glm::vec3 position = c.GetPosition();
    glm::vec4 clipPos =
        mvp * glm::vec4(position.x, position.y, position.z, 1.0f);
    glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
    sp_enemy->SetTranslate(glm::vec2(ndc.x, ndc.y) / 8.0f);
    sp_enemy->Render();
  }
  sp_player->SetTranslate(glm::vec2(0.0f));
  sp_player->Render();
}

void ModelManager::RenderMapPoints(const std::vector<MapPoint>& map_points,
                                   glm::vec4 color) {
  for (int i = 0; i < map_points.size(); ++i) {
    // not instancesd draw call, but separate class MapPoint
    if (map_points[i].selected) {
      map_point_.Select();
    } else {
      map_point_.DeSelect();
    }
    map_point_.Render(ui_shared_resources_, color, map_points[i].position);
  }
}

void ModelManager::RenderCreaturesAsMapPoints(int creature_id,
                                              glm::vec4 color) {
  if (creature_id == -1) {
    for (auto& c : creatures_) {
      if (c.IsSelected()) {
        map_point_.Select();
      } else {
        map_point_.DeSelect();
      }
      map_point_.Render(ui_shared_resources_, color, c.GetPosition());
    }
  } else {
    const auto& creature = creatures_[creature_id];
    if (creature.IsSelected()) {
      map_point_.Select();
    } else {
      map_point_.DeSelect();
    }
    map_point_.Render(ui_shared_resources_, color, creature.GetPosition());
  }
}

void ModelManager::RenderPickingCreaturesAsMapPoints(int creature_id) {
  if (creature_id == -1) {
    for (int i = 0; i < creatures_.size(); ++i) {
      map_point_.RenderPicking(ui_shared_resources_, i, creatures_[i].GetPosition());
    }
  } else {
    const auto& creature = creatures_[creature_id];
    map_point_.RenderPicking(ui_shared_resources_, creature_id, creature.GetPosition());
  }
}

void ModelManager::RenderPickingMapPoints(
    const std::vector<MapPoint>& map_points) {
  for (int i = 0; i < map_points.size(); ++i) {
    // not instancesd draw call, but separate class MapPoint
    if (map_points[i].selected) {
      map_point_.Select();
    } else {
      map_point_.DeSelect();
    }
    map_point_.RenderPicking(ui_shared_resources_, details::kIdOffsetObjects + i,
      map_points[i].position);
  }
}

void ModelManager::RenderMapPoints(const std::vector<MapPoint>& map_points,
                                   const std::vector<glm::quat>& rotates,
                                   const std::vector<glm::vec3>& scales,
                                   glm::vec4 color) {
  for (int i = 0; i < map_points.size(); ++i) {
    // not instancesd draw call, but separate class MapPoint
    if (map_points[i].selected) {
      map_point_.Select();
    } else {
      map_point_.DeSelect();
    }
    map_point_.Render(ui_shared_resources_, color,
      map_points[i].position, rotates[i], scales[i]);
  }
}

void ModelManager::RenderPickingMapPoints(
    const std::vector<MapPoint>& map_points,
    const std::vector<glm::quat>& rotates,
    const std::vector<glm::vec3>& scales) {
  for (int i = 0; i < map_points.size(); ++i) {
    // not instancesd draw call, but separate class MapPoint
    if (map_points[i].selected) {
      map_point_.Select();
    } else {
      map_point_.DeSelect();
    }
    map_point_.RenderPicking(ui_shared_resources_, details::kIdOffsetObjects + i,
      map_points[i].position, rotates[i], scales[i]);
  }
}

void ModelManager::RenderPicking() {
  for (auto& c : creatures_) {
    c.RenderPicking(ui_shared_resources_);
  }
  player_->RenderPicking(ui_shared_resources_);
}

void ModelManager::Update() {
  for (auto& c : creatures_) {
    c.Update(ui_shared_resources_);
  }
  //  for (auto& o : obstacles_) {
  //    o.Update(ui_shared_resources_);
  //  }
  player_->Update(ui_shared_resources_);
  glNamedBufferSubData(player_ubo_, 0, sizeof(glm::vec3),
    glm::value_ptr(player_->GetPosition()));


  std::unordered_map<uint32_t, RigidBody*> entities;
  for (auto& c : creatures_) {
    entities[c.GetId()] = &c;
  }
  entities[player_human_.GetId()] = &player_human_;
  entities[player_fpv_.GetId()] = &player_fpv_;

  std::vector<Collider> colliders;
  colliders.reserve(entities.size());
  for (const auto& e : entities) {
    colliders.push_back({e.second->GenWorldAabb(), e.first});
  }

  std::vector<glm::vec3> aabb_centers(entities.size(), glm::vec3(0.0f));
  for (int i = 0; i < colliders.size(); ++i) {
    const auto& aabb = colliders[i].aabb;
    aabb_centers[i] = 0.5f * (aabb.min + aabb.max);
  }

  std::vector<BVHNode> bvh;
  BuildBVH(bvh, aabb_centers, colliders, 0, colliders.size());

  for (const auto& attack : attack_queue_) {
    std::vector<int> hits;
    QueryBVH(bvh, 0, attack.hitbox, hits);
    for (auto hit : hits) {
      if (hit != attack.attacker) {
        entities[hit]->Stunned();
      }
    }
  }
  attack_queue_.clear();
}

Aabb3D ModelManager::ComputeBounds(const std::vector<Collider>& colliders,
                                   int start, int end) {
  Aabb3D aabb;
  for (int i = start; i < end; ++i) {
    aabb = Aabb3D::Combine(aabb, colliders[i].aabb);
  }
  return aabb;
}

int ModelManager::BuildBVH(std::vector<BVHNode>& bvh,
                           const std::vector<glm::vec3>& aabb_centers,
                           std::vector<Collider>& colliders, int start,
                           int end) {
  BVHNode node;
  node.bounds = ComputeBounds(colliders, start, end);

  int nodeIndex = bvh.size();
  bvh.push_back(node);

  int count = end - start;
  if (count == 1) {
    bvh[nodeIndex].object = static_cast<int>(colliders[start].owner);
    return nodeIndex;
  }

  int axis = node.bounds.LongestAxis();
  int mid = (start + end) / 2;

  std::nth_element(colliders.begin() + start, colliders.begin() + mid,
                   colliders.begin() + end, [&](const auto& a, const auto& b) {
                     return aabb_centers[a.owner][axis] <
                            aabb_centers[b.owner][axis];
                   });

  bvh[nodeIndex].left = BuildBVH(bvh, aabb_centers, colliders, start, mid);
  bvh[nodeIndex].right = BuildBVH(bvh, aabb_centers, colliders, mid, end);
  return nodeIndex;
}

void ModelManager::QueryBVH(const std::vector<BVHNode>& bvh, int node,
                            const Aabb3D& query, std::vector<int>& hits) {
  const BVHNode& n = bvh[node];
  if (!Aabb3D::Intersect(n.bounds, query)) return;

  if (n.object != -1) {
    hits.push_back(n.object);
    return;
  }

  QueryBVH(bvh, n.left, query, hits);
  QueryBVH(bvh, n.right, query, hits);
}
