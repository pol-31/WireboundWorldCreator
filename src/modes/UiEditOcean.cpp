#include "UiEditOcean.h"

#include "../core/TileRenderer.h"

UiEditOcean::UiEditOcean(UiRenderData& render_data,
                         TextRenderer& text_renderer,
                         UiEditSlots& ui_edit_slots,
                         UiEditConfigSlCfg& value_config,
                         UiConfigWindow& ui_ocean_config)
    : IUiEdit(ui_edit_slots),
      value_config_(value_config),
      ocean_layers_(
          {{data::TextId::kLayer1,
            {100.0f, 20'000.0f, 1'0000, 0.6f, 0.7f, 5.0f, 0.7f, 1.1f}},
           {data::TextId::kLayer2,
            {100.0f, 20'000.0f, 1'0000, 0.6f, 0.7f, 5.0f, 0.7f, 1.1f}},
           {data::TextId::kLayer3,
            {100.0f, 20'000.0f, 1'0000, 0.6f, 0.7f, 5.0f, 0.7f, 1.1f}}}),
      render_data_(render_data),
      ui_ocean_config_(ui_ocean_config) {}

void UiEditOcean::HideAll() {
  ui_.ForceHide();
  ui_ocean_config_.ForceHide();
}

void UiEditOcean::CreateInstance() {
  Data().push_back(OceanTraits{});
  UpdateConfig();
}

void UiEditOcean::UpdateConfig() {
  render_data_.glfw_context_.tile_renderer->water.SetWaterColor(
      Data()[selected_id_].color);
}

void UiEditOcean::SetInstanceId(int id) {
  ui_.SetInstance(id);
  auto& ocean_data = Data()[id];
  ocean_layers_[0].SetConfig(&ocean_data.near);
  ocean_layers_[1].SetConfig(&ocean_data.mid);
  ocean_layers_[2].SetConfig(&ocean_data.far);
  UpdateConfig();
}

void UiEditOcean::RemoveInstance(GLuint id) {
  Data().erase(Data().begin() + id);
  UpdateConfig();
}

void UiEditOcean::Reset() { Data().clear(); }

void UiEditOcean::Generate() {
  OceanTraits traits;
  traits.near = ocean_layers_[0].GetConfig();
  traits.mid = ocean_layers_[1].GetConfig();
  traits.far = ocean_layers_[2].GetConfig();
  TileRenderer* tile_renderer =
      render_data_.glfw_context_.tile_renderer;
  tile_renderer->water.UpdateOcean(traits);
}

void UiEditOcean::GenerateAll() {
  for (int id = 0; id < Data().size(); ++id) {
    auto& ocean_data = Data()[id];
    ocean_layers_[0].SetConfig(&ocean_data.near);
    ocean_layers_[1].SetConfig(&ocean_data.mid);
    ocean_layers_[2].SetConfig(&ocean_data.far);
    OceanTraits traits;
    traits.near = ocean_layers_[0].GetConfig();
    traits.mid = ocean_layers_[1].GetConfig();
    traits.far = ocean_layers_[2].GetConfig();
    TileRenderer* tile_renderer =
        render_data_.glfw_context_.tile_renderer;
    tile_renderer->water.UpdateOcean(traits);
    render_data_.glfw_context_.tile_renderer->water.SetWaterColor(
        Data()[id].color);
  }
}

void UiEditOcean::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  for (auto& n : ocean_layers_) {
    n.Randomize(ui_.random_generator_, dist_float);
  }
  Generate();
  UpdateConfig();
}

bool UiEditOcean::Press(int id, float height) {
  value_config_.ResetTransform();
  int pressed_line_id =
      GetUiEditEntryId(height, ocean_layers_.size(),
                       render_data_.glfw_context_.cursor_pos_tex_norm_,
                       value_config_.btn_config_.GetTopBorder());
  if (id == value_config_.btn_config_.GetId()) {
    ui_ocean_config_.SetNoise(ocean_layers_[pressed_line_id].GetValueSpan(),
                              ocean_layers_[pressed_line_id].GetTextIdSpan(),
                              ocean_layers_[pressed_line_id].GetTextId());
    ui_ocean_config_.Show();
  } else if (id == value_config_.sl_strength_.GetId()) {
    value_config_.pressed_strength_id_ = pressed_line_id;
    value_config_.sl_strength_.Press();
  } else if (id == value_config_.tg1_.GetId()) {
    ocean_layers_[pressed_line_id].ToggleVisible();
  } else if (id == value_config_.tg2_.GetId()) {
    ocean_layers_[pressed_line_id].ToggleVisible2();
  } else {
    return false;
  }
  return true;
}

void UiEditOcean::Release() { value_config_.Release(); }

void UiEditOcean::Render(float height) {
  render_data_.glfw_context_.tile_renderer->water.SetWaterColor(
      Data()[selected_id_].color);
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = height / (ocean_layers_.size() + 5);  // +pads
  for (int i = 0; i < ocean_layers_.size(); ++i) {
    value_config_.Render(
        ocean_layers_[i].GetStrengthRef(), ocean_layers_[i].GetVisible(),
        ocean_layers_[i].GetVisible2(), next_offset,
        value_config_.pressed_strength_id_ == i, ocean_layers_[i].GetTextId());
    next_offset.y -= entry_height;
  }
}

void UiEditOcean::RenderPicking(float height) {
  value_config_.RenderPicking(height, ocean_layers_.size());
}

OceanTraits& UiEditOcean::GetInstanceData() noexcept {
  return Data()[selected_id_];
}

std::vector<OceanTraits>& UiEditOcean::Data() {
  return render_data_.glfw_context_.tile_renderer->cur_tile_.ocean_data;
}
