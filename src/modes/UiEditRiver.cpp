#include "UiEditRiver.h"

#include "../core/TileRenderer.h"

GraphBakeConfig RiverTraits::GetGraphConfig() const noexcept {
  return {.curve_amplitude = config.curve_amplitude,
          .height_drift = config.height_drift,
          .height_raise = config.height_raise,
          .side_sagging = config.side_sagging,
          .radius = config.radius,
          .radius_flat = config.radius_flat};
}

UiEditRiver::UiEditRiver(UiRenderData& render_data, UiEditSlots& ui_edit_slots,
                         UiEditConfigSlTxt& value_config)
    : IUiEdit(ui_edit_slots),
      value_config_(value_config),
      render_data_(render_data) {}

void UiEditRiver::HideAll() { ui_.ForceHide(); }

void UiEditRiver::CreateInstance() {
  Data().push_back(RiverTraits{});
  UpdateConfig();
}

void UiEditRiver::UpdateConfig() {
  if (selected_id_ == -1) {
    return;
  }
  auto& inst = Data()[selected_id_];
  traits_ = {{
      {&inst.transparency, data::TextId::kWaterTransparency},
      {&inst.viscosity, data::TextId::kWaterViscosity},
      {&inst.config.curve_amplitude, data::TextId::kWaterCurveAmplitude},
      {&inst.config.height_drift, data::TextId::kWaterHeightDrift},
      {&inst.config.height_raise, data::TextId::kWaterHeightRaise},
      {&inst.config.side_sagging, data::TextId::kWaterSideSagging},
      {&inst.config.radius, data::TextId::kWaterRadius},
      {&inst.config.radius_flat, data::TextId::kWaterRadiusFlat},
  }};
  render_data_.glfw_context_.tile_renderer->UpdatePipeline();
}

void UiEditRiver::SetInstanceId(int id) {
  ui_.SetInstance(id);
  UpdateConfig();
}

void UiEditRiver::RemoveInstance(GLuint id) {
  Data().erase(Data().begin() + id);
  UpdateConfig();
}

void UiEditRiver::Reset() {
  Data().clear();
  UpdateConfig();
}

void UiEditRiver::Generate() { UpdateConfig(); }

void UiEditRiver::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  auto& gen = ui_.random_generator_;
  Data()[selected_id_].transparency = dist_float(gen);
  Data()[selected_id_].viscosity = dist_float(gen);
  Data()[selected_id_].config.curve_amplitude = dist_float(gen) * 10.0f;
  Data()[selected_id_].config.height_drift = dist_float(gen) * 0.5f;
  Data()[selected_id_].config.height_raise = dist_float(gen) - 1.0f;
  Data()[selected_id_].config.side_sagging = dist_float(gen) * 0.5f;
  Data()[selected_id_].config.radius = dist_float(gen) * 10.0f;
  Data()[selected_id_].config.radius_flat = dist_float(gen) * 5.0f;
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
  return Data()[selected_id_];
}

std::vector<RiverTraits>& UiEditRiver::Data() {
  return render_data_.glfw_context_.tile_renderer->cur_tile_.rivers_data;
}
