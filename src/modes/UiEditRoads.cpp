#include "UiEditRoads.h"

#include "../core/TileRenderer.h"

GraphBakeConfig RoadTraits::GetGraphConfig() const noexcept {
  return {.curve_amplitude = config.curve_amplitude,
          .height_drift = config.height_drift,
          .height_raise = config.height_raise,
          .side_sagging = config.side_sagging,
          .radius = config.radius,
          .radius_flat = config.radius_flat};
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
      value_config_(other.value_config_),
      ui_shared_resources_(other.ui_shared_resources_) {
  // river_layer_config_.AttachToHierarchy(hierarchy_);
}

void UiEditRoads::HideAll() { ui_.ForceHide(); }

void UiEditRoads::CreateInstance() {
  Data().push_back(RoadTraits{});
  UpdateConfig();
}

void UiEditRoads::UpdateConfig() {
  if (selected_id_ == -1) {
    return;
  }
  auto& inst = Data()[selected_id_];
  traits_ = {{
      {&inst.config.curve_amplitude, data::TextId::kPlacementCurveAmplitude},
      {&inst.config.height_drift, data::TextId::kPlacementHeightDrift},
      {&inst.config.height_raise, data::TextId::kPlacementHeightRaise},
      {&inst.config.side_sagging, data::TextId::kPlacementSideSagging},
      {&inst.config.radius, data::TextId::kPlacementRadius},
      {&inst.config.radius_flat, data::TextId::kPlacementRadiusFlat},
  }};
  ui_shared_resources_.glfw_context_.tile_renderer->UpdatePipeline();
}

void UiEditRoads::SetInstanceId(int id) {
  ui_.SetInstance(id);
  UpdateConfig();
}

void UiEditRoads::RemoveInstance(GLuint id) {
  Data().erase(Data().begin() + id);
  UpdateConfig();
}

void UiEditRoads::Reset() {
  Data().clear();
  UpdateConfig();
}

void UiEditRoads::Generate() { UpdateConfig(); }

void UiEditRoads::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  auto& gen = ui_.random_generator_;
  Data()[selected_id_].config.curve_amplitude = dist_float(gen);
  Data()[selected_id_].config.curve_amplitude = dist_float(gen) * 10.0f;
  Data()[selected_id_].config.height_drift = dist_float(gen) * 0.5f;
  Data()[selected_id_].config.height_raise = dist_float(gen) * 2.0f - 1.0f;
  Data()[selected_id_].config.side_sagging = dist_float(gen) * 0.5f;
  Data()[selected_id_].config.radius = dist_float(gen) * 10.0f;
  Data()[selected_id_].config.radius_flat = dist_float(gen) * 5.0f;
  Generate();
}

bool UiEditRoads::Press(int id, float height) {
  return value_config_.Press(id, height, traits_.size());
}

void UiEditRoads::Release() { value_config_.Release(); }

void UiEditRoads::Render(float height) {
  value_config_.Render(height, traits_);
}

void UiEditRoads::RenderPicking(float height) {
  value_config_.RenderPicking(height, traits_.size());
}

RoadTraits& UiEditRoads::GetInstanceData() noexcept {
  return Data()[selected_id_];
}

std::vector<RoadTraits>& UiEditRoads::Data() {
  return ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.roads_data;
}
