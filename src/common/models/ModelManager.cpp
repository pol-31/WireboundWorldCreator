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
      animator_(loader_, animation_ubo_),
      player_(ui_shared_resources),
      shader_aabb_("../shaders/Aabb.vert", "../shaders/Aabb.frag"),
      shader_aabb_picking_("../shaders/Aabb.vert", "../shaders/ModelPicking.frag") {
  Init();
}

void ModelManager::Init() {
  glCreateBuffers(1, &player_ubo_);
  glNamedBufferStorage(player_ubo_, sizeof(glm::vec3), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glBindBufferBase(GL_UNIFORM_BUFFER, 4, player_ubo_);

  glGenBuffers(1, &animation_ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, animation_ubo_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * Animator::gMaxBones,
               nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 10, animation_ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  loader_.SetImageLoader(LoadImageData, nullptr);

  animator_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Human.gltf");

  auto mdl_tree =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\MapMarker.gltf", 1);
  auto mdl_human =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Human.gltf", 2);
  auto mdl_fpv =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Fpv.gltf", 3);
  mdl_aabb_ = mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Cube.gltf", 4);

  player_.SetModelData(mdl_human, &attack_queue_);
  player_.SetAnimator(&animator_);
  player_.GetFpv().SetModelData(mdl_fpv, &attack_queue_);

  int creatures_num = 3;
  for (int i = 0; i < creatures_num; ++i) {
    creatures_.emplace_back();
    creatures_[i].SetModelData(mdl_human, &attack_queue_);
    creatures_[i].SetAnimator(&animator_);
    creatures_[i].SetPosition(glm::vec3(i + 1, 0.0f, i));
  }
  int fpvs_num = 2;
  for (int i = 0; i < fpvs_num; ++i) {
    fpvs_.emplace_back();
    fpvs_[i].SetModelData(mdl_fpv, &attack_queue_);
    fpvs_[i].SetPosition(glm::vec3(i - 1, 0.0f, i));
  }
  tree_.SetModelData(mdl_tree);
  bush_.SetModelData(mdl_tree);
  tall_grass_.SetModelData(mdl_tree);
  undergrowth_.SetModelData(mdl_tree);
  map_point_.SetModel(mdl_tree);
}

void ModelManager::DeInit() {
  glDeleteBuffers(1, &player_ubo_);
  glDeleteBuffers(1, &animation_ubo_);
}

void ModelManager::Render() {
  for (auto& c : creatures_) {
    c.Render(ui_shared_resources_);
  }
  for (auto& c : fpvs_) {
    c.Render(ui_shared_resources_);
  }
  tree_.Render(ui_shared_resources_);
  bush_.Render(ui_shared_resources_);
  tall_grass_.Render(ui_shared_resources_);
  undergrowth_.Render(ui_shared_resources_);

  player_.Render(ui_shared_resources_);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_CULL_FACE);
  shader_aabb_.Bind();
  const auto& prim = mdl_aabb_->primitives[0];
  glBindVertexArray(prim.vao);
  auto color_white = glm::vec4(1.0f, 1.0f, 1.0f, 0.6f);
  glUniform4fv(4, 1, glm::value_ptr(color_white));
  for (const auto& entity : entities_) {
    RenderAabb(entity.second, prim);
  }

  if (player_.IsFpv()) {
    const auto& fpv_pos = player_.GetFpv().GetPosition();
    //TODO: not that velocity, but cur_pos rel to prev_pos
    //TODO: not that velocity, but cur_pos rel to prev_pos
    //TODO: not that velocity, but cur_pos rel to prev_pos
    const auto& fpv_vel = player_.GetFpv().GetVelocity();
    auto hit = ui_shared_resources_.glfw_context_.tile_renderer
      ->CastRay(fpv_pos, glm::normalize(fpv_vel), 16.0f);
    if (hit != glm::vec3(-1000.0f)) {
      auto map_scale =
          ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.map_scale;
      glm::mat4 object_model = glm::mat4{1.0f};
      object_model = glm::translate(object_model, hit);
      object_model *= glm::mat4_cast(player_.GetFpv().GetRotation());
      object_model = glm::scale(object_model, player_.GetFpv().GetScale());
      glm::mat4 map_model =
          glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));  // upscaled
      auto model_mat = map_model * object_model;

      glUniformMatrix4fv(0, 1, false, glm::value_ptr(model_mat));
      const auto& aabb = player_.GetFpv().GetModelData()->aabb;
      glm::vec3 sizes = (aabb.max - aabb.min) / 2.0f;
      glm::vec3 center = (aabb.min + aabb.max) / 2.0f;
      glUniform3fv(2, 1, glm::value_ptr(sizes));
      glUniform3fv(3, 1, glm::value_ptr(center));
      auto color_red = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
      glUniform4fv(4, 1, glm::value_ptr(color_red));
      glDrawElements(
        prim.mode, prim.indexCount, prim.indexType,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(prim.indexOffset)));
    }
  }
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModelManager::RenderPlacement() {
  tree_.Render(ui_shared_resources_);
  bush_.Render(ui_shared_resources_);
  tall_grass_.Render(ui_shared_resources_);
  undergrowth_.Render(ui_shared_resources_);
}

void ModelManager::RenderOnMap(UiSprite* sp_player, UiSprite* sp_enemy,
                               UiSprite* sp_friend, UiSprite* sp_neutral,
                               UiSprite* sp_obstacle) {
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
  for (auto& c : fpvs_) {
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
  shader_aabb_picking_.Bind();
  const auto& prim = mdl_aabb_->primitives[0];
  glBindVertexArray(prim.vao);
  if (creature_id == -1) {
    for (int i = 0; i < creatures_.size(); ++i) {
      glUniform1ui(1, static_cast<uint32_t>(i));
      RenderAabb(&map_point_, prim, creatures_[i].GetPosition());
    }
  } else {
    const auto& creature = creatures_[creature_id];
    glUniform1ui(1, static_cast<uint32_t>(creature_id));
    RenderAabb(&map_point_, prim, creature.GetPosition());
  }
}

void ModelManager::RenderPickingMapPoints(
const std::vector<MapPoint>& map_points) {
  shader_aabb_picking_.Bind();
  const auto& prim = mdl_aabb_->primitives[0];
  glBindVertexArray(prim.vao);
  for (int i = 0; i < map_points.size(); ++i) {
    glUniform1ui(1, static_cast<uint32_t>(details::kIdOffsetObjects + i));
    RenderAabb(&map_point_, prim, map_points[i].position);
  }
}

void ModelManager::RenderMapPoints(const std::vector<MapPoint>& map_points,
                                   const std::vector<glm::quat>& rotates,
                                   const std::vector<glm::vec3>& scales,
                                   glm::vec4 color) {
  for (int i = 0; i < map_points.size(); ++i) {
    if (map_points[i].selected) {
      map_point_.Select();
    } else {
      map_point_.DeSelect();
    }
    map_point_.Render(ui_shared_resources_, color, map_points[i].position,
                      rotates[i], scales[i]);
  }
}

void ModelManager::RenderPickingMapPoints(
    const std::vector<MapPoint>& map_points,
    const std::vector<glm::quat>& rotates,
    const std::vector<glm::vec3>& scales) {
  shader_aabb_picking_.Bind();
  const auto& prim = mdl_aabb_->primitives[0];
  glBindVertexArray(prim.vao);
  for (int i = 0; i < map_points.size(); ++i) {
    glUniform1ui(1, static_cast<uint32_t>(details::kIdOffsetObjects + i));
    RenderAabb(&map_point_, prim, map_points[i].position, rotates[i], scales[i]);
  }
}

void ModelManager::RenderPicking() {
  shader_aabb_picking_.Bind();
  const auto& prim = mdl_aabb_->primitives[0];
  glBindVertexArray(prim.vao);
  glEnable(GL_CULL_FACE);
  for (const auto& entity : entities_) {
    glUniform1ui(1, static_cast<uint32_t>(details::kIdOffsetObjects + entity.first));
    RenderAabb(entity.second, prim);
  }
  glDisable(GL_CULL_FACE);
}

void ModelManager::Update() {
  for (auto& c : creatures_) {
    c.Update(ui_shared_resources_);
  }
  for (auto& c : fpvs_) {
    c.Update(ui_shared_resources_);
  }
  player_.Update(ui_shared_resources_);

  glNamedBufferSubData(player_ubo_, 0, sizeof(glm::vec3),
                       glm::value_ptr(player_.GetPosition()));
  UpdateBvh();
  ProcessEvents();
}

void ModelManager::UpdateBvh() {
  entities_.clear();
  for (auto& c : creatures_) {
    entities_[c.GetId()] = &c;
  }
  for (auto& c : fpvs_) {
    entities_[c.GetId()] = &c;
  }
  if (player_.IsFpv()) {
    entities_[player_.GetFpv().GetId()] = &player_.GetFpv();
  } else {
    entities_[player_.GetId()] = &player_;
  }

  std::vector<Collider> colliders;
  colliders.reserve(entities_.size());
  for (const auto& e : entities_) {
    colliders.push_back({e.second->GenWorldAabb(), e.first});
  }
  std::vector<glm::vec3> aabb_centers(entities_.size(), glm::vec3(0.0f));
  for (int i = 0; i < colliders.size(); ++i) {
    const auto& aabb = colliders[i].aabb;
    aabb_centers[i] = 0.5f * (aabb.min + aabb.max);
  }

  bvh_.clear();
  BuildBVH(bvh_, aabb_centers, colliders, 0, colliders.size());
}

void ModelManager::ProcessEvents() {
  for (const auto& attack : attack_queue_) {
    std::vector<int> hits;
    QueryBVH(bvh_, 0, attack.hitbox, hits);
    for (auto hit : hits) {
      if (hit != attack.attacker) {
        entities_[hit]->Stunned();
      }
    }
  }
  attack_queue_.clear();
}

void ModelManager::RenderAabb(RigidBody* entity, const ModelData::Mesh& prim) {
  auto model_mat = entity->GenModelMat(ui_shared_resources_, 1.0f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model_mat));
  const auto& aabb = entity->GetModelData()->aabb;
  glm::vec3 sizes = (aabb.max - aabb.min) / 2.0f;
  glm::vec3 center = (aabb.min + aabb.max) / 2.0f;
  glUniform3fv(2, 1, glm::value_ptr(sizes));
  glUniform3fv(3, 1, glm::value_ptr(center));
  glDrawElements(
    prim.mode, prim.indexCount, prim.indexType,
    reinterpret_cast<void*>(static_cast<std::uintptr_t>(prim.indexOffset)));
}

void ModelManager::RenderAabb(
    MapMarker* entity, const ModelData::Mesh& prim,
    glm::vec2 position, glm::quat rotation,
    glm::vec3 scale) {
  auto model_mat = entity->GenModelMat(ui_shared_resources_, position, rotation, scale);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model_mat));
  const auto& aabb = entity->GetModelData()->aabb;
  glm::vec3 sizes = (aabb.max - aabb.min) / 2.0f;
  glm::vec3 center = (aabb.min + aabb.max) / 2.0f;
  glUniform3fv(2, 1, glm::value_ptr(sizes));
  glUniform3fv(3, 1, glm::value_ptr(center));
  glDrawElements(
    prim.mode, prim.indexCount, prim.indexType,
    reinterpret_cast<void*>(static_cast<std::uintptr_t>(prim.indexOffset)));
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
