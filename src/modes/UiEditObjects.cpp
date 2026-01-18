#include "UiEditObjects.h"

UiEditObjects::UiEditObjects(UiSharedResources& ui_shared_resources,
                             UiEditSlots& ui_edit_slots,
                             UiEditConfigSlTxt& value_config,
                             ModelManager& mdl_manager)
    : IUiEdit(ui_edit_slots),
      value_config_(value_config),
      mdl_manager_(mdl_manager) {
  // value_config_.AttachToHierarchy(hierarchy_);
}

UiEditObjects::UiEditObjects(UiEditObjects&& other) noexcept
    : IUiEdit(std::move(*this)),
      instances_(std::move(other.instances_)),
      value_config_(other.value_config_),
      mdl_manager_(other.mdl_manager_) {
  // value_config_.AttachToHierarchy(hierarchy_);
}

void UiEditObjects::HideAll() { ui_.ForceHide(); }

void UiEditObjects::CreateInstance() {
  instances_.push_back(ObjectTraits{});
  UpdateConfig();
}

void UiEditObjects::UpdateConfig() {
  int id = *ui_.selected_id_;
  if (id == -1) {
    return;
  }
  auto& inst = instances_[id];
  traits_ = {{
      {&inst.hp, data::TextId::kStrength},
      {&inst.speed, data::TextId::kStrength},
      {&inst.attack, data::TextId::kStrength},
      {&inst.attack_speed, data::TextId::kStrength},
  }};
  const auto& model = mdl_manager_.GetLoadedModels()[id];
  model->hp = instances_[id].hp;
  model->speed = instances_[id].speed;
  model->attack = instances_[id].attack;
  model->attack_speed = instances_[id].attack_speed;
  std::cout << "Objects characteristics updated:" << ' ' << instances_[id].hp
            << ' ' << instances_[id].speed << ' ' << instances_[id].attack
            << ' ' << instances_[id].attack_speed << std::endl;
}

void UiEditObjects::SetInstanceId(int id) {
  ui_.SetInstance(id);
  UpdateConfig();
}

void UiEditObjects::RemoveInstance(GLuint id) {
  instances_.erase(instances_.begin() + id);
  UpdateConfig();
}

void UiEditObjects::Reset() {
  instances_.clear();
  UpdateConfig();
}

void UiEditObjects::Generate() { UpdateConfig(); }

void UiEditObjects::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  int id = *ui_.selected_id_;
  auto& gen = ui_.random_generator_;
  instances_[id].hp = dist_float(gen);
  instances_[id].speed = dist_float(gen);
  instances_[id].attack = dist_float(gen);
  instances_[id].attack_speed = dist_float(gen);
  Generate();
}

bool UiEditObjects::Press(int id, float height) {
  return value_config_.Press(id, height, traits_.size());
}

void UiEditObjects::Release() { value_config_.Release(); }

void UiEditObjects::Render(float height) {
  value_config_.Render(height, traits_);
}

void UiEditObjects::RenderPicking(float height) {
  value_config_.RenderPicking(height, traits_.size());
}

ObjectTraits& UiEditObjects::GetInstanceData() noexcept {
  return instances_[*ui_.selected_id_];
}
