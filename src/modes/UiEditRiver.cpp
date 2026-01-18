#include "UiEditRiver.h"

#include "../core/TileRenderer.h"

GraphBakeConfig RiverTraits::GetGraphConfig() const noexcept {
  return {.curve_amplitude = curve_amplitude,
          .height_drift = height_drift,
          .height_raise = height_raise,
          .side_sagging = side_sagging,
          .radius_flat = radius_flat,
          .radius = radius};
}

UiEditRiver::UiEditRiver(UiSharedResources& ui_shared_resources,
                         UiEditSlots& ui_edit_slots,
                         UiEditConfigSlTxt& value_config)
    : IUiEdit(ui_edit_slots),
      value_config_(value_config),
      ui_shared_resources_(ui_shared_resources) {
  // river_layer_config_.AttachToHierarchy(hierarchy_);
}

UiEditRiver::UiEditRiver(UiEditRiver&& other) noexcept
    : IUiEdit(std::move(*this)),
      instances_(std::move(other.instances_)),
      value_config_(other.value_config_),
      ui_shared_resources_(other.ui_shared_resources_) {
  // river_layer_config_.AttachToHierarchy(hierarchy_);
}

void UiEditRiver::HideAll() { ui_.ForceHide(); }

void UiEditRiver::CreateInstance() {
  instances_.push_back(RiverTraits{});
  UpdateConfig();
}

void UiEditRiver::UpdateConfig() {
  int id = *ui_.selected_id_;
  if (id == -1) {
    return;
  }
  auto& inst = instances_[id];
  traits_ = {{
      {&inst.transparency, data::TextId::kStrength},
      {&inst.viscosity, data::TextId::kStrength},
      {&inst.curve_amplitude, data::TextId::kStrength},
      {&inst.height_drift, data::TextId::kStrength},
      {&inst.height_raise, data::TextId::kStrength},
      {&inst.side_sagging, data::TextId::kStrength},
      {&inst.radius_flat, data::TextId::kStrength},
      {&inst.radius, data::TextId::kStrength},
  }};
  ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.rivers_ =
      &instances_;
  ui_shared_resources_.glfw_context_.tile_renderer->UpdatePipeline();
}

void UiEditRiver::SetInstanceId(int id) {
  ui_.SetInstance(id);
  UpdateConfig();
}

void UiEditRiver::RemoveInstance(GLuint id) {
  instances_.erase(instances_.begin() + id);
  UpdateConfig();
}

void UiEditRiver::Reset() {
  instances_.clear();
  UpdateConfig();
}

void UiEditRiver::Generate() { UpdateConfig(); }

void UiEditRiver::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  int id = *ui_.selected_id_;
  auto& gen = ui_.random_generator_;
  instances_[id].transparency = dist_float(gen);
  instances_[id].viscosity = dist_float(gen);
  instances_[id].curve_amplitude = dist_float(gen) * 10.0f;
  instances_[id].height_drift = dist_float(gen) * 0.5f;
  instances_[id].height_raise = dist_float(gen) - 1.0f;
  instances_[id].side_sagging = dist_float(gen) * 0.5f;
  instances_[id].radius_flat = dist_float(gen) * 5.0f;
  instances_[id].radius = dist_float(gen) * 10.0f;
  Generate();
}

bool UiEditRiver::Press(int id, float height) {
  return value_config_.Press(id, height, traits_.size());
}

void UiEditRiver::Release() { value_config_.Release(); }

void UiEditRiver::Render(float height) {
  value_config_.Render(height, traits_);
}

void UiEditRiver::RenderPicking(float height) {
  value_config_.RenderPicking(height, traits_.size());
}

RiverTraits& UiEditRiver::GetInstanceData() noexcept {
  return instances_[*ui_.selected_id_];
}
