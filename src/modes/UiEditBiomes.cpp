#include "UiEditBiomes.h"

UiEditBiomes::UiEditBiomes(UiSharedResources& ui_shared_resources,
                           UiEditSlots& ui_edit_slots,
                           UiEditConfigSlTxt& value_config)
    : IUiEdit(ui_edit_slots), value_config_(value_config) {
  // value_config_.AttachToHierarchy(ui_.hierarchy_);
}

UiEditBiomes::UiEditBiomes(UiEditBiomes&& other) noexcept
    : IUiEdit(std::move(*this)),
      instances_(std::move(other.instances_)),
      value_config_(other.value_config_) {
  // value_config_.AttachToHierarchy(ui_.hierarchy_);
}

void UiEditBiomes::HideAll() { ui_.ForceHide(); }

void UiEditBiomes::CreateInstance() {
  instances_.push_back(BiomeTraits{});
  UpdateConfig();
}

void UiEditBiomes::UpdateConfig() {
  int id = *ui_.selected_id_;
  if (id == -1) {
    return;
  }
  auto& inst = instances_[id];
  traits_ = {{
      {&inst.wind_angle, data::TextId::kStrength},
      {&inst.wind_speed, data::TextId::kStrength},
      {&inst.sun_x, data::TextId::kStrength},
      {&inst.sun_y, data::TextId::kStrength},
      {&inst.sun_z, data::TextId::kStrength},
      {&inst.sun_r, data::TextId::kStrength},
      {&inst.sun_g, data::TextId::kStrength},
      {&inst.sun_b, data::TextId::kStrength},
  }};
  glm::vec3 sun_direction = {inst.sun_x, inst.sun_y, inst.sun_z};
  glm::vec3 sun_color = {inst.sun_r, inst.sun_g, inst.sun_b};
  const auto& biome = instances_[id];
  ui_.ui_shared_resources_.glfw_context_.tile_renderer->environment_
      .SetEnvironment(biome.wind_angle, biome.wind_speed, sun_direction,
                      sun_color);
}

void UiEditBiomes::SetInstanceId(int id) {
  ui_.SetInstance(id);
  UpdateConfig();
}

void UiEditBiomes::RemoveInstance(GLuint id) {
  instances_.erase(instances_.begin() + id);
  UpdateConfig();
}

void UiEditBiomes::Reset() {
  instances_.clear();
  UpdateConfig();
}

void UiEditBiomes::Generate() { UpdateConfig(); }

void UiEditBiomes::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  int id = *ui_.selected_id_;
  auto& gen = ui_.random_generator_;
  instances_[id].wind_angle = dist_float(gen);
  instances_[id].wind_speed = dist_float(gen);
  instances_[id].sun_x = dist_float(gen);
  instances_[id].sun_y = dist_float(gen);
  instances_[id].sun_z = dist_float(gen);
  instances_[id].sun_r = dist_float(gen);
  instances_[id].sun_g = dist_float(gen);
  instances_[id].sun_b = dist_float(gen);
  Generate();
}

bool UiEditBiomes::Press(int id, float height) {
  return value_config_.Press(id, height, traits_.size());
}

void UiEditBiomes::Release() { value_config_.Release(); }

void UiEditBiomes::Render(float height) {
  value_config_.Render(height, traits_);
}

void UiEditBiomes::RenderPicking(float height) {
  value_config_.RenderPicking(height, traits_.size());
}

BiomeTraits& UiEditBiomes::GetInstanceData() noexcept {
  return instances_[*ui_.selected_id_];
}
