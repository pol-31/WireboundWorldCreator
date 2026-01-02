#include "UiEditBiomes.h"

UiEditBiomes::UiEditBiomes(UiSharedResources& ui_shared_resources,
                           WindowQueue& window_queue,
                           TextRenderer& text_renderer,
                           std::vector<BaseInstanceData>& base_instances,
                           const int& selected_id)
    : Base({data::VboIdMain::kBiomesEditDesk}, 1.0f,
           {{data::VboIdMain::kBiomesEditDeskPinBack, []() {}},
            {data::VboIdMain::kBiomesEditDeskPinPoint}},
           ui_shared_resources, window_queue),
      accept_(data::VboIdMain::kBiomesEditAccept),
      name_(text_renderer, {data::VboIdMain::kBiomesEditNameBack},
            {data::VboIdMain::kBiomesEditName}),
      color_palette_({data::VboIdMain::kBiomesColorPaletteHS},
                     {data::VboIdMain::kBiomesColorColorCursor},
                     glm::vec2{1.0f}),
      color_brightness_({data::VboIdMain::kBiomesColorPaletteB},
                        {data::VboIdMain::kBiomesColorBrightnessCursor}, 1.0f),
      color_indicator_(data::VboIdMain::kBiomesColorIndicator),
      random_generate_(data::VboIdMain::kBiomesEditRandomGeneration,
                       [this]() { this->RandomGenerate(); }),
      ui_event_handler_({&pin_, &value_config_.sl_strength_, &accept_, &name_,
                         &color_palette_, &color_brightness_,
                         &random_generate_}),
      ui_shared_resources_(ui_shared_resources),
      random_generator_(std::random_device{}()),
      base_instances_(base_instances),
      selected_id_(selected_id),
      value_config_(ui_shared_resources, text_renderer,
                    {{data::VboIdMain::kBiomesEditNoiseStrengthArea},
                     {data::VboIdMain::kBiomesEditNoiseStrengthIcon}},
                    {text_renderer,
                     {data::VboIdMain::kBiomesEditNoiseName},
                     data::TextId::kPerlin}) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &accept_, &name_, &color_palette_,
                  &color_brightness_, &color_indicator_, &random_generate_);
  value_config_.AttachToHierarchy(hierarchy_);
  speed_ = 2.0f;
}

UiEditBiomes::UiEditBiomes(UiEditBiomes&& other) noexcept
    : Base(std::move(other)),
      accept_(std::move(other.accept_)),
      name_(std::move(other.name_)),
      color_palette_(std::move(other.color_palette_)),
      color_brightness_(std::move(other.color_brightness_)),
      color_indicator_(std::move(other.color_indicator_)),
      random_generate_(std::move(other.random_generate_)),
      ui_event_handler_({&pin_, &value_config_.sl_strength_, &accept_, &name_,
                         &color_palette_, &color_brightness_,
                         &random_generate_}),
      ui_shared_resources_(other.ui_shared_resources_),
      instances_(std::move(other.instances_)),
      base_instances_(other.base_instances_),
      selected_id_(other.selected_id_),
      random_generator_(other.random_generator_),
      value_config_(std::move(other.value_config_)) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &accept_, &name_, &color_palette_,
                  &color_brightness_, &color_indicator_, &random_generate_);
  value_config_.AttachToHierarchy(hierarchy_);
}

void UiEditBiomes::HideAll() { ForceHide(); }

void UiEditBiomes::CreateInstance() { instances_.push_back(BiomeTraits{}); }

void UiEditBiomes::UpdateConfig() {}

void UiEditBiomes::SetInstanceId(int id) {}

bool UiEditBiomes::Press(int id) {
  if (id == sprite_.GetId()) {
    return true;
  }
  if (id == pin_.GetId()) {
    pin_.Press();
    return true;
  }
  if (id == accept_.GetId()) {
    std::cout << "SAVE MODEL CONFIG" << std::endl;
    Generate();
    return true;
  }
  value_config_.ResetTransform();
  int pressed_line_id = GetUiEditEntryId(
      &sprite_, 2,  // size 2: transparency, viscosity
      ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_,
      value_config_.sl_strength_.GetTrackPtr()->GetTopBorder());
  if (id == value_config_.sl_strength_.GetId()) {
    value_config_.pressed_strength_id_ = pressed_line_id;
    value_config_.sl_strength_.Press();
    return true;
  }
  return ui_event_handler_.Press(id);
}

void UiEditBiomes::Release() {
  value_config_.pressed_strength_id_ = -1;
  value_config_.sl_strength_.Release();
  ui_event_handler_.Release();
}

bool UiEditBiomes::Scroll(GLuint id, float yoffset) {
  return color_palette_.Scroll(id, yoffset) ||
         color_brightness_.Scroll(id, yoffset);
}

bool UiEditBiomes::Render() {
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

  auto& model_data = base_instances_[selected_id_];

  model_data.color =
      HSBtoRGB(color_palette_.GetProgressX() * 0.85f,
               color_palette_.GetProgressY(), color_brightness_.GetProgress());
  glUniform4fv(7, 1, glm::value_ptr(model_data.color));
  color_indicator_.Render();
  auto color = glm::vec4{1.0f};
  glUniform4fv(7, 1, glm::value_ptr(color));

  random_generate_.Render();
  name_.RenderBack();

  RenderModelConfig();

  // text section

  model_data.name = name_.GetText();
  name_.RenderText();

  return false;
}

void UiEditBiomes::RenderPicking() {
  RenderPickingBack();
  ui_shared_resources_.shader_sp_picking_.Bind();

  accept_.RenderPicking();
  name_.RenderPicking();
  color_palette_.RenderPicking();
  color_brightness_.RenderPicking();
  color_indicator_.RenderPicking();
  random_generate_.RenderPicking();

  RenderPickingModelConfig();

  ui_shared_resources_.shader_sp_picking_.Bind();
  name_.RenderPicking();
}

void UiEditBiomes::Generate() {
  const auto& biome = instances_[selected_id_];
  ui_shared_resources_.gltf_context_.tile_renderer->environment_.SetEnvironment(
      biome.wind_speed, biome.wind_angle, biome.sun_direction, biome.sun_color);
}

void UiEditBiomes::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  instances_[selected_id_].wind_speed = dist_float(random_generator_);
  instances_[selected_id_].wind_angle = dist_float(random_generator_);
  instances_[selected_id_].sun_direction.x = dist_float(random_generator_);
  instances_[selected_id_].sun_direction.y = dist_float(random_generator_);
  instances_[selected_id_].sun_direction.z = dist_float(random_generator_);
  instances_[selected_id_].sun_color.x = dist_float(random_generator_);
  instances_[selected_id_].sun_color.y = dist_float(random_generator_);
  instances_[selected_id_].sun_color.z = dist_float(random_generator_);
  Generate();
}

BiomeTraits& UiEditBiomes::GetInstanceData() noexcept {
  return instances_[selected_id_];
}

void UiEditBiomes::RenderGraph() {
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

void UiEditBiomes::Reset() { instances_.clear(); }

void UiEditBiomes::MergeLayers() {}  // TODO:

void UiEditBiomes::RenderModelConfig() {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = sprite_.GetHeight() / (8 + 5);  // +pads
  value_config_.Render(instances_[selected_id_].wind_speed, next_offset,
                       value_config_.pressed_strength_id_ == 0,
                       data::TextId::kWindSpeed);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].wind_angle, next_offset,
                       value_config_.pressed_strength_id_ == 1,
                       data::TextId::kWindAngle);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].sun_direction.x, next_offset,
                       value_config_.pressed_strength_id_ == 2,
                       data::TextId::kSunX);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].sun_direction.y, next_offset,
                       value_config_.pressed_strength_id_ == 3,
                       data::TextId::kSunY);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].sun_direction.z, next_offset,
                       value_config_.pressed_strength_id_ == 4,
                       data::TextId::kSunZ);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].sun_color.x, next_offset,
                       value_config_.pressed_strength_id_ == 5,
                       data::TextId::kSunR);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].sun_color.y, next_offset,
                       value_config_.pressed_strength_id_ == 6,
                       data::TextId::kSunG);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].sun_color.z, next_offset,
                       value_config_.pressed_strength_id_ == 7,
                       data::TextId::kSunB);
}

void UiEditBiomes::RenderPickingModelConfig() {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = sprite_.GetHeight() / (8 + 5);  // +pads
  for (int i = 0; i < 8; ++i) {
    value_config_.RenderPicking(next_offset);
    next_offset.y -= entry_height;
  }
}
