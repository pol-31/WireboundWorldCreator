#include "ModelManager.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../../core/Ui.h"
#include "../../io/Camera.h"

ModelManager::ModelManager(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      player_(ui_shared_resources),
      mdl_loader_(ui_shared_resources) {
  auto mdl_musca =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Musca.gltf", 1);
  auto mdl_snowman =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\NpcBoy.gltf", 2);

  auto mdl_tree =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Tree.gltf", 3);
  auto mdl_bush =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\Bush.gltf", 4);
  auto mdl_tall_grass =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\assets\\TallGrass.gltf", 5);
  auto mdl_undergrowth = mdl_loader_.Load(
      "C:\\Users\\Pavlo\\Desktop\\assets\\Undergrowth.gltf", 6);

  player_.SetModelData(mdl_snowman);
  int creatures_num = 3;
  for (int i = 0; i < creatures_num; ++i) {
    creatures_.emplace_back();
    creatures_[i].SetModelData(mdl_musca);
    creatures_[i].SetPosition(glm::vec3(i + 1, 0.0f, i));
  }
  tree_.SetModelData(mdl_tree);
  bush_.SetModelData(mdl_bush);
  tall_grass_.SetModelData(mdl_tall_grass);
  undergrowth_.SetModelData(mdl_undergrowth);
  map_point_.SetModelData(mdl_tree);
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
  player_.Render(ui_shared_resources_);
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
      ui_shared_resources_.gltf_context_.tile_renderer->cur_tile_.map_scale;
  glm::mat4 model;
  auto view =
      ui_shared_resources_.gltf_context_.camera->GetViewMatrix(map_scale);
  auto projection = ui_shared_resources_.gltf_context_.camera->GetProjMatrix();
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

void ModelManager::RenderPicking() {
  for (auto& c : creatures_) {
    c.RenderPicking(ui_shared_resources_);
  }
}

void ModelManager::Update() {
  for (auto& c : creatures_) {
    c.Update(ui_shared_resources_);
  }
  //  for (auto& o : obstacles_) {
  //    o.Update(ui_shared_resources_);
  //  }
  player_.Update(ui_shared_resources_);
}

void ModelManager::Spawn(GLuint vao_id, glm::vec2 position) {
  const auto& models = mdl_loader_.GetLoadedModels();
  for (const auto& m : models) {
    if (m->vao != vao_id) {
      continue;
    }
    creatures_.emplace_back();
    creatures_.back().SetPosition(glm::vec3(position.x, 0.0f, position.y));
    creatures_.back().UpdatePositionY(ui_shared_resources_);
    creatures_.back().SetModelData(m.get());
    return;
  }
}

void ModelManager::Kill(int id) {}
