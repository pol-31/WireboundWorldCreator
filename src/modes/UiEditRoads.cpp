#include "UiEditRoads.h"

#include "../core/TileRenderer.h"

GraphBakeConfig RoadTraits::GetGraphConfig() const noexcept {
  return {
    .curve_amplitude = curve_amplitude,
    .height_drift = height_drift,
    .height_raise = height_raise,
    .side_sagging = side_sagging,
    .radius_flat = radius_flat,
    .radius = radius
  };
}

UiEditRoads::UiEditRoads(UiSharedResources& ui_shared_resources,
                         UiEditSlots& ui_edit_slots,
           UiEditConfigSlTxt& value_config)
    : IUiEdit(ui_edit_slots),
value_config_(value_config),
  ui_shared_resources_(ui_shared_resources) {
  // river_layer_config_.AttachToHierarchy(hierarchy_);
}

UiEditRoads::UiEditRoads(UiEditRoads&& other) noexcept
    : IUiEdit(std::move(*this)),
instances_(std::move(other.instances_)),
      value_config_(other.value_config_),
      ui_shared_resources_(other.ui_shared_resources_) {
  // river_layer_config_.AttachToHierarchy(hierarchy_);
}

void UiEditRoads::HideAll() {
  ui_.ForceHide();
}

void UiEditRoads::CreateInstance() {
  instances_.push_back(RoadTraits{});
  UpdateConfig();
}

void UiEditRoads::UpdateConfig() {
  int id = *ui_.selected_id_;
  if (id == -1) {
    return;
  }
  auto& inst = instances_[id];
  traits_ = {{
    {&inst.curve_amplitude, data::TextId::kStrength},
    {&inst.height_drift, data::TextId::kStrength},
    {&inst.height_raise, data::TextId::kStrength},
    {&inst.side_sagging, data::TextId::kStrength},
    {&inst.radius_flat, data::TextId::kStrength},
    {&inst.radius, data::TextId::kStrength},
  }};
  ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.roads_ = &instances_;
  ui_shared_resources_.glfw_context_.tile_renderer->UpdatePipeline();
}

void UiEditRoads::SetInstanceId(int id) {
  ui_.SetInstance(id);
  UpdateConfig();
}

void UiEditRoads::RemoveInstance(GLuint id) {
  instances_.erase(instances_.begin() + id);
  UpdateConfig();
}

void UiEditRoads::Reset() {
  instances_.clear();
  UpdateConfig();
}

void UiEditRoads::Generate() {
  UpdateConfig();
}

void UiEditRoads::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  int id = *ui_.selected_id_;
  auto& gen = ui_.random_generator_;
  instances_[id].curve_amplitude = dist_float(gen);
  instances_[id].curve_amplitude = dist_float(gen) * 10.0f;
  instances_[id].height_drift = dist_float(gen) * 0.5f;
  instances_[id].height_raise = dist_float(gen) * 2.0f - 1.0f;
  instances_[id].side_sagging = dist_float(gen) * 0.5f;
  instances_[id].radius_flat = dist_float(gen) * 5.0f;
  instances_[id].radius = dist_float(gen) * 10.0f;
  Generate();
}

bool UiEditRoads::Press(int id, float height) {
  return value_config_.Press(id, height, traits_.size());
}

void UiEditRoads::Release() {
  value_config_.Release();
}

void UiEditRoads::Render(float height) {
  value_config_.Render(height, traits_);
}

void UiEditRoads::RenderPicking(float height) {
  value_config_.RenderPicking(height, traits_.size());
}

RoadTraits& UiEditRoads::GetInstanceData() noexcept {
  return instances_[*ui_.selected_id_];
}
