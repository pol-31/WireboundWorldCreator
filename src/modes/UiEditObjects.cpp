#include "UiEditObjects.h"

UiEditObjects::UiEditObjects(UiSharedResources& ui_shared_resources,
                             UiEditSlots& ui_edit_slots,
                             UiEditConfigSlTxt& value_config,
                             ModelManager& mdl_manager)
    : IUiEdit(ui_edit_slots),
      value_config_(value_config),
      mdl_manager_(mdl_manager),
      ui_shared_resources_(ui_shared_resources) {}

void UiEditObjects::HideAll() { ui_.ForceHide(); }

void UiEditObjects::CreateInstance() {
  Data().push_back(ObjectTraits{});
  UpdateConfig();
}

void UiEditObjects::UpdateConfig() {
  if (selected_id_ == -1) {
    return;
  }
  auto& inst = Data()[selected_id_];
  traits_ = {{
      {&inst.hp, data::TextId::kHp},
      {&inst.speed, data::TextId::kSpeed},
      {&inst.attack, data::TextId::kAttack},
      {&inst.attack_speed, data::TextId::kAttackSpeed},
  }};
  const auto& model = mdl_manager_.GetLoadedModels()[selected_id_];
  model->hp = inst.hp;
  model->speed = inst.speed;
  model->attack = inst.attack;
  model->attack_speed = inst.attack_speed;
  std::cout << "Objects characteristics updated:" << ' ' << inst.hp << ' '
            << inst.speed << ' ' << inst.attack << ' ' << inst.attack_speed
            << std::endl;
}

void UiEditObjects::SetInstanceId(int id) {
  ui_.SetInstance(id);
  UpdateConfig();
}

void UiEditObjects::RemoveInstance(GLuint id) {
  Data().erase(Data().begin() + id);
  UpdateConfig();
}

void UiEditObjects::Reset() {
  Data().clear();
  UpdateConfig();
}

void UiEditObjects::Generate() { UpdateConfig(); }

void UiEditObjects::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  auto& gen = ui_.random_generator_;
  Data()[selected_id_].hp = dist_float(gen);
  Data()[selected_id_].speed = dist_float(gen);
  Data()[selected_id_].attack = dist_float(gen);
  Data()[selected_id_].attack_speed = dist_float(gen);
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
  return Data()[selected_id_];
}

void UiEditObjects::SetModels(
    const std::vector<std::unique_ptr<ModelData>>& models) {
  Data().clear();
  for (const auto& m : models) {
    ObjectTraits object;
    object.name = m->name;
    Data().push_back(object);
  }
  UpdateConfig();
}

std::vector<ObjectTraits>& UiEditObjects::Data() {
  return ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_
      .objects_data;
}
