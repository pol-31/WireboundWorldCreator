#include "UiEditBiomes.h"

UiEditBiomes::UiEditBiomes(UiRenderData& render_data,
                           UiEditSlots& ui_edit_slots,
                           UiEditConfigSlTxt& value_config)
    : IUiEdit(ui_edit_slots),
      value_config_(value_config),
      render_data_(render_data) {}

void UiEditBiomes::HideAll() { ui_.ForceHide(); }

void UiEditBiomes::CreateInstance() {
  Data().push_back(BiomeTraits{});
  UpdateConfig();
}

void UiEditBiomes::UpdateConfig() {
  if (selected_id_ == -1) {
    return;
  }
  auto& inst = Data()[selected_id_];
  traits_ = {{
      {&inst.wind_angle, data::TextId::kWindAngle},
      {&inst.wind_speed, data::TextId::kWindSpeed},
      {&inst.sun_x, data::TextId::kSunOffsetX},
      {&inst.sun_y, data::TextId::kSunOffsetZ},
      {&inst.sun_z, data::TextId::kSunOffsetZ},
      {&inst.sun_r, data::TextId::kSunColorR},
      {&inst.sun_g, data::TextId::kSunColorG},
      {&inst.sun_b, data::TextId::kSunColorB},
  }};
  glm::vec3 sun_direction = {inst.sun_x, inst.sun_y, inst.sun_z};
  glm::vec3 sun_color = {inst.sun_r, inst.sun_g, inst.sun_b};
  const auto& biome = Data()[selected_id_];
  ui_.render_data_.glfw_context_.tile_renderer->environment_
      .SetEnvironment(biome.wind_angle, biome.wind_speed, sun_direction,
                      sun_color);
}

void UiEditBiomes::SetInstanceId(int id) {
  ui_.SetInstance(id);
  UpdateConfig();
}

void UiEditBiomes::RemoveInstance(GLuint id) {
  Data().erase(Data().begin() + id);
  UpdateConfig();
}

void UiEditBiomes::Reset() {
  Data().clear();
  UpdateConfig();
}

void UiEditBiomes::Generate() { UpdateConfig(); }

void UiEditBiomes::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  auto& gen = ui_.random_generator_;
  Data()[selected_id_].wind_angle = dist_float(gen);
  Data()[selected_id_].wind_speed = dist_float(gen);
  Data()[selected_id_].sun_x = dist_float(gen);
  Data()[selected_id_].sun_y = dist_float(gen);
  Data()[selected_id_].sun_z = dist_float(gen);
  Data()[selected_id_].sun_r = dist_float(gen);
  Data()[selected_id_].sun_g = dist_float(gen);
  Data()[selected_id_].sun_b = dist_float(gen);
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
  return Data()[selected_id_];
}

std::vector<BiomeTraits>& UiEditBiomes::Data() {
  return render_data_.glfw_context_.tile_renderer->cur_tile_
      .biomes_data;
}
