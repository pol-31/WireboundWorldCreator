#include "ModelManager.h"

ModelManager::ModelManager(
    UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      player_(ui_shared_resources),
      mdl_loader_(ui_shared_resources) {
  auto mdl_musca =
      mdl_loader_.Load("C:\\Users\\Pavlo\\Desktop\\musca2.gltf", 1);
  int creatures_num = 3;
  for (int i = 0; i < creatures_num; ++i) {
    creatures_.emplace_back();
    creatures_[i].SetModelData(mdl_musca);
    creatures_[i].SetPosition(glm::vec3(i, 0.0f, i));
  }
  player_.SetModelData(mdl_musca);
}

void ModelManager::Render() {
  for (auto& c : creatures_) {
    c.Render(ui_shared_resources_);
  }
//  for (auto& o : obstacles_) {
//    o.Render(ui_shared_resources_);
//  }
  player_.Render(ui_shared_resources_);
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

void ModelManager::Spawn(const ModelData& model_data, size_t num) {}

void ModelManager::Kill(int id) {}
