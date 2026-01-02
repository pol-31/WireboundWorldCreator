#include "UiEditWater.h"

UiEditOcean::UiEditOcean(UiSharedResources& ui_shared_resources,
                         WindowQueue& window_queue, TextRenderer& text_renderer,
                         std::vector<BaseInstanceData>& base_instances,
                         const int& selected_id,
                         UiConfigWindow& ui_ocean_config)
    : Base({data::VboIdMain::kWaterEditDesk}, 1.0f,
           {{data::VboIdMain::kWaterEditDeskPinBack, []() {}},
            {data::VboIdMain::kWaterEditDeskPinPoint}},
           ui_shared_resources, window_queue),
      accept_(data::VboIdMain::kWaterEditAccept),
      name_(text_renderer, {data::VboIdMain::kWaterEditNameBack},
            {data::VboIdMain::kWaterEditName}),
      color_palette_({data::VboIdMain::kWaterColorPaletteHS},
                     {data::VboIdMain::kWaterColorColorCursor},
                     glm::vec2{1.0f}),
      color_brightness_({data::VboIdMain::kWaterColorPaletteB},
                        {data::VboIdMain::kWaterColorBrightnessCursor}, 1.0f),
      color_indicator_(data::VboIdMain::kWaterColorIndicator),
      random_generate_(data::VboIdMain::kWaterEditRandomGeneration,
                       [this]() { this->RandomGenerate(); }),
      ocean_layer_config_(ui_shared_resources, text_renderer),
      ocean_layers_(
          {{data::TextId::kLayer1,
            {100.0f, 20'000.0f, 1'0000, 0.6f, 0.7f, 5.0f, 0.7f, 1.1f}},
           {data::TextId::kLayer2,
            {100.0f, 20'000.0f, 1'0000, 0.6f, 0.7f, 5.0f, 0.7f, 1.1f}},
           {data::TextId::kLayer3,
            {100.0f, 20'000.0f, 1'0000, 0.6f, 0.7f, 5.0f, 0.7f, 1.1f}}}),

      ui_event_handler_({&pin_, &ocean_layer_config_.btn_config_,
                         &ocean_layer_config_.tg_visible_, &accept_, &name_,
                         &color_palette_, &color_brightness_,
                         &random_generate_}),
      ui_shared_resources_(ui_shared_resources),
      random_generator_(std::random_device{}()),
      base_instances_(base_instances),
      selected_id_(selected_id),
      ui_ocean_config_(ui_ocean_config) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &accept_, &name_, &color_palette_,
                  &color_brightness_, &color_indicator_, &random_generate_);
  ocean_layer_config_.AttachToHierarchy(hierarchy_);
  speed_ = 2.0f;
  color_brightness_.SetValue(1.0f);
}

UiEditOcean::UiEditOcean(UiEditOcean&& other) noexcept
    : Base(std::move(other)),
      accept_(std::move(other.accept_)),
      name_(std::move(other.name_)),
      color_palette_(std::move(other.color_palette_)),
      color_brightness_(std::move(other.color_brightness_)),
      color_indicator_(std::move(other.color_indicator_)),
      random_generate_(std::move(other.random_generate_)),

      ocean_layer_config_(std::move(other.ocean_layer_config_)),
      ocean_layers_(std::move(other.ocean_layers_)),
      ui_event_handler_({&pin_, &ocean_layer_config_.btn_config_,
                         &ocean_layer_config_.tg_visible_, &accept_, &name_,
                         &color_palette_, &color_brightness_,
                         &random_generate_}),
      ui_shared_resources_(other.ui_shared_resources_),
      instances_(std::move(other.instances_)),
      base_instances_(other.base_instances_),
      selected_id_(other.selected_id_),
      random_generator_(other.random_generator_),
      ui_ocean_config_(other.ui_ocean_config_) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &accept_, &name_, &color_palette_,
                  &color_brightness_, &color_indicator_, &random_generate_);
  ocean_layer_config_.AttachToHierarchy(hierarchy_);
}

void UiEditOcean::HideAll() {
  ForceHide();
  ui_ocean_config_.ForceHide();
}

void UiEditOcean::CreateInstance() { instances_.push_back(OceanTraits{}); }

void UiEditOcean::UpdateConfig() {}

void UiEditOcean::SetInstanceId(int id) {
  const auto& ocean_data = instances_[id];
  ocean_layers_[0].SetConfig(&ocean_data.near);
  ocean_layers_[1].SetConfig(&ocean_data.mid);
  ocean_layers_[2].SetConfig(&ocean_data.far);
  ui_shared_resources_.gltf_context_.tile_renderer->water.SetWaterColor(
      base_instances_[selected_id_].color);
}

bool UiEditOcean::Press(int id) {
  if (id == sprite_.GetId()) {
    return true;
  }
  if (id == pin_.GetId()) {
    pin_.Press();
    return true;
  }
  if (id == accept_.GetId()) {
    std::cout << "GENERATE OCEAN" << std::endl;
    Generate();
    return true;
  }
  ocean_layer_config_.ResetTransform();
  int pressed_line_id =
      GetUiEditEntryId(&sprite_, ocean_layers_.size(),
                       ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_,
                       ocean_layer_config_.btn_config_.GetTopBorder());
  if (id == ocean_layer_config_.btn_config_.GetId()) {
    ui_ocean_config_.SetNoise(ocean_layers_[pressed_line_id].GetValueSpan(),
                              ocean_layers_[pressed_line_id].GetTextIdSpan(),
                              ocean_layers_[pressed_line_id].GetTextId());
    ui_ocean_config_.Show();
    return true;
  } else if (id == ocean_layer_config_.tg_visible_.GetId()) {
    ocean_layers_[pressed_line_id].ToggleVisible();
  }
  return ui_event_handler_.Press(id);
}

void UiEditOcean::Release() { ui_event_handler_.Release(); }

bool UiEditOcean::Scroll(GLuint id, float yoffset) {
  return color_palette_.Scroll(id, yoffset) ||
         color_brightness_.Scroll(id, yoffset);
}

bool UiEditOcean::Render() {
  ui_shared_resources_.tex_ui_.Bind();
  auto mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_;
  bool stop_show = Base::RenderBack(true);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  accept_.Render();
  color_palette_.Render(mouse_pos);
  color_brightness_.Render(mouse_pos);

  auto& ocean_data = base_instances_[selected_id_];

  ocean_data.color =
      HSBtoRGB(color_palette_.GetProgressX() * 0.85f,
               color_palette_.GetProgressY(), color_brightness_.GetProgress());
  glUniform4fv(7, 1, glm::value_ptr(ocean_data.color));
  color_indicator_.Render();
  auto color = glm::vec4{1.0f};
  glUniform4fv(7, 1, glm::value_ptr(color));

  random_generate_.Render();
  name_.RenderBack();

  RenderOceanConfig();

  // text section

  ocean_data.name = name_.GetText();
  name_.RenderText();

  return false;
}

void UiEditOcean::RenderPicking() {
  RenderPickingBack();
  ui_shared_resources_.shader_sp_picking_.Bind();

  accept_.RenderPicking();
  name_.RenderPicking();
  color_palette_.RenderPicking();
  color_brightness_.RenderPicking();
  color_indicator_.RenderPicking();
  random_generate_.RenderPicking();

  RenderPickingOceanConfig();

  ui_shared_resources_.shader_sp_picking_.Bind();
  name_.RenderPicking();
}

void UiEditOcean::Generate() {
  OceanTraits traits{
      0.0f,
      {},
      ocean_layers_[0].GetConfig(),
      ocean_layers_[1].GetConfig(),
      ocean_layers_[2].GetConfig(),
  };
  instances_[selected_id_].near = ocean_layers_[0].GetConfigUnscaled();
  instances_[selected_id_].mid = ocean_layers_[1].GetConfigUnscaled();
  instances_[selected_id_].far = ocean_layers_[2].GetConfigUnscaled();
  TileRenderer* tile_renderer =
      ui_shared_resources_.gltf_context_.tile_renderer;
  tile_renderer->water.UpdateOcean(traits);
}

void UiEditOcean::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  for (auto& n : ocean_layers_) {
    n.Randomize(random_generator_, dist_float);
  }
  Generate();
}

OceanTraits& UiEditOcean::GetInstanceData() noexcept {
  return instances_[selected_id_];
}

void UiEditOcean::RenderGraph() {
  /*if (selected_id_ == -1) {
    return;
  }
  /// selected layer wireframe
  ui_shared_resources_.gltf_context_.tile_renderer
      ->terrain.RenderWireframe(&instances_[selected_id_],
                                &base_instances_[selected_id_]);
  auto color_invert = glm::vec4(1.0f) - base_instances_[selected_id_].color;
  color_invert.w = 1.0f;

  /// wireframe left bottom
  auto& ui_layer_wireframe =
      ui_shared_resources_.gltf_context_
          .ui_renderer->GetUiLayerWireframe();
  ui_layer_wireframe.RenderLayerWireframe(
      &instances_[selected_id_], &base_instances_[selected_id_]);*/
}

void UiEditOcean::Reset() { instances_.clear(); }

void UiEditOcean::MergeLayers() {}  // TODO:

void UiEditOcean::RenderOceanConfig() {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height =
      sprite_.GetHeight() / (ocean_layers_.size() + 5);  // +pads
  for (int i = 0; i < ocean_layers_.size(); ++i) {
    ocean_layer_config_.Render(ocean_layers_[i].GetVisible(), next_offset,
                               ocean_layers_[i].GetTextId());
    next_offset.y -= entry_height;
  }
}

void UiEditOcean::RenderPickingOceanConfig() {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height =
      sprite_.GetHeight() / (ocean_layers_.size() + 5);  // +pads
  for (int i = 0; i < ocean_layers_.size(); ++i) {
    ocean_layer_config_.RenderPicking(next_offset);
    next_offset.y -= entry_height;
  }
}

UiEditRiver::UiEditRiver(UiSharedResources& ui_shared_resources,
                         WindowQueue& window_queue, TextRenderer& text_renderer,
                         std::vector<BaseInstanceData>& base_instances,
                         const int& selected_id)
    : Base({data::VboIdMain::kWaterEditDesk}, 1.0f,
           {{data::VboIdMain::kWaterEditDeskPinBack, []() {}},
            {data::VboIdMain::kWaterEditDeskPinPoint}},
           ui_shared_resources, window_queue),
      accept_(data::VboIdMain::kWaterEditAccept),
      name_(text_renderer, {data::VboIdMain::kWaterEditNameBack},
            {data::VboIdMain::kWaterEditName}),
      color_palette_({data::VboIdMain::kWaterColorPaletteHS},
                     {data::VboIdMain::kWaterColorColorCursor},
                     glm::vec2{1.0f}),
      color_brightness_({data::VboIdMain::kWaterColorPaletteB},
                        {data::VboIdMain::kWaterColorBrightnessCursor}, 1.0f),
      color_indicator_(data::VboIdMain::kWaterColorIndicator),
      random_generate_(data::VboIdMain::kWaterEditRandomGeneration,
                       [this]() { this->RandomGenerate(); }),
      ui_event_handler_({&pin_, &river_layer_config_.sl_strength_, &accept_,
                         &name_, &color_palette_, &color_brightness_,
                         &random_generate_}),
      ui_shared_resources_(ui_shared_resources),
      random_generator_(std::random_device{}()),
      base_instances_(base_instances),
      selected_id_(selected_id),
      river_layer_config_(ui_shared_resources, text_renderer,
                          {{data::VboIdMain::kWaterEditRiverTransparencyArea},
                           {data::VboIdMain::kWaterEditRiverTransparencyIcon}},
                          {text_renderer,
                           {data::VboIdMain::kWaterEditRiverTransparencyText},
                           data::TextId::kPerlin}) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &accept_, &name_, &color_palette_,
                  &color_brightness_, &color_indicator_, &random_generate_);
  river_layer_config_.AttachToHierarchy(hierarchy_);
  speed_ = 2.0f;
  color_brightness_.SetValue(1.0f);
}

UiEditRiver::UiEditRiver(UiEditRiver&& other) noexcept
    : Base(std::move(other)),
      accept_(std::move(other.accept_)),
      name_(std::move(other.name_)),
      color_palette_(std::move(other.color_palette_)),
      color_brightness_(std::move(other.color_brightness_)),
      color_indicator_(std::move(other.color_indicator_)),
      random_generate_(std::move(other.random_generate_)),
      ui_event_handler_({&pin_, &river_layer_config_.sl_strength_, &accept_,
                         &name_, &color_palette_, &color_brightness_,
                         &random_generate_}),
      ui_shared_resources_(other.ui_shared_resources_),
      instances_(std::move(other.instances_)),
      base_instances_(other.base_instances_),
      selected_id_(other.selected_id_),
      random_generator_(other.random_generator_),
      river_layer_config_(std::move(other.river_layer_config_)) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &accept_, &name_, &color_palette_,
                  &color_brightness_, &color_indicator_, &random_generate_);
  river_layer_config_.AttachToHierarchy(hierarchy_);
}

void UiEditRiver::HideAll() { ForceHide(); }

void UiEditRiver::CreateInstance() { instances_.push_back(RiverTraits{}); }

void UiEditRiver::UpdateConfig() {}

void UiEditRiver::SetInstanceId(int id) {}

bool UiEditRiver::Press(int id) {
  if (id == sprite_.GetId()) {
    return true;
  }
  if (id == pin_.GetId()) {
    pin_.Press();
    return true;
  }
  if (id == accept_.GetId()) {
    std::cout << "GENERATE RIVER" << std::endl;
    Generate();
    return true;
  }
  river_layer_config_.ResetTransform();
  int pressed_line_id = GetUiEditEntryId(
      &sprite_, 2,  // size 2: transparency, viscosity
      ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_,
      river_layer_config_.sl_strength_.GetTrackPtr()->GetTopBorder());
  if (id == river_layer_config_.sl_strength_.GetId()) {
    river_layer_config_.pressed_strength_id_ = pressed_line_id;
    river_layer_config_.sl_strength_.Press();
    return true;
  }
  return ui_event_handler_.Press(id);
}

void UiEditRiver::Release() {
  river_layer_config_.pressed_strength_id_ = -1;
  river_layer_config_.sl_strength_.Release();
  ui_event_handler_.Release();
}

bool UiEditRiver::Scroll(GLuint id, float yoffset) {
  return color_palette_.Scroll(id, yoffset) ||
         color_brightness_.Scroll(id, yoffset);
}

bool UiEditRiver::Render() {
  ui_shared_resources_.tex_ui_.Bind();
  auto mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_;
  bool stop_show = Base::RenderBack(true);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  accept_.Render();
  color_palette_.Render(mouse_pos);
  color_brightness_.Render(mouse_pos);

  auto& river_data = base_instances_[selected_id_];

  river_data.color =
      HSBtoRGB(color_palette_.GetProgressX() * 0.85f,
               color_palette_.GetProgressY(), color_brightness_.GetProgress());
  glUniform4fv(7, 1, glm::value_ptr(river_data.color));
  color_indicator_.Render();
  auto color = glm::vec4{1.0f};
  glUniform4fv(7, 1, glm::value_ptr(color));

  random_generate_.Render();
  name_.RenderBack();

  RenderRiverConfig();

  // text section

  river_data.name = name_.GetText();
  name_.RenderText();

  return false;
}

void UiEditRiver::RenderPicking() {
  RenderPickingBack();
  ui_shared_resources_.shader_sp_picking_.Bind();

  accept_.RenderPicking();
  name_.RenderPicking();
  color_palette_.RenderPicking();
  color_brightness_.RenderPicking();
  color_indicator_.RenderPicking();
  random_generate_.RenderPicking();

  RenderPickingRiverConfig();

  ui_shared_resources_.shader_sp_picking_.Bind();
  name_.RenderPicking();
}

void UiEditRiver::Generate() {
  std::cerr << "River generation isn't implemented" << std::endl;
}

void UiEditRiver::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  instances_[selected_id_].transparency = dist_float(random_generator_);
  instances_[selected_id_].viscosity = dist_float(random_generator_);
  Generate();
}

RiverTraits& UiEditRiver::GetInstanceData() noexcept {
  return instances_[selected_id_];
}

void UiEditRiver::RenderGraph() {
  /*if (selected_id_ == -1) {
    return;
  }
  /// selected layer wireframe
  ui_shared_resources_.gltf_context_.tile_renderer
      ->terrain.RenderWireframe(&instances_[selected_id_],
                                &base_instances_[selected_id_]);
  auto color_invert = glm::vec4(1.0f) - base_instances_[selected_id_].color;
  color_invert.w = 1.0f;

  /// wireframe left bottom
  auto& ui_layer_wireframe =
      ui_shared_resources_.gltf_context_
          .ui_renderer->GetUiLayerWireframe();
  ui_layer_wireframe.RenderLayerWireframe(
      &instances_[selected_id_], &base_instances_[selected_id_]);*/
}

void UiEditRiver::Reset() { instances_.clear(); }

void UiEditRiver::MergeLayers() {}  // TODO:

void UiEditRiver::RenderRiverConfig() {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = sprite_.GetHeight() / (2 + 5);  // +pads
  river_layer_config_.Render(instances_[selected_id_].transparency, next_offset,
                             river_layer_config_.pressed_strength_id_ == 0,
                             data::TextId::kStrength);
  next_offset.y -= entry_height;
  river_layer_config_.Render(instances_[selected_id_].viscosity, next_offset,
                             river_layer_config_.pressed_strength_id_ == 1,
                             data::TextId::kSwell);
}

void UiEditRiver::RenderPickingRiverConfig() {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = sprite_.GetHeight() / (2 + 5);  // +pads
  river_layer_config_.RenderPicking(next_offset);
  next_offset.y -= entry_height;
  river_layer_config_.RenderPicking(next_offset);
  next_offset.y -= entry_height;
}
