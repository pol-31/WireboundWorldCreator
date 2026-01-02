#include "UiEditObjects.h"

UiEditObjects::UiEditObjects(UiSharedResources& ui_shared_resources,
                             WindowQueue& window_queue,
                             TextRenderer& text_renderer,
                             std::vector<BaseInstanceData>& base_instances,
                             const int& selected_id, ModelManager& mdl_manager)
    : Base({data::VboIdMain::kObjectsEditDesk}, 1.0f,
           {{data::VboIdMain::kObjectsEditDeskPinBack, []() {}},
            {data::VboIdMain::kObjectsEditDeskPinPoint}},
           ui_shared_resources, window_queue),
      accept_(data::VboIdMain::kObjectsEditAccept),
      name_(text_renderer, {data::VboIdMain::kObjectsEditNameBack},
            {data::VboIdMain::kObjectsEditName}),
      random_generate_(data::VboIdMain::kObjectsEditRandomGeneration,
                       [this]() { this->RandomGenerate(); }),
      ui_event_handler_({&pin_, &value_config_.sl_strength_, &accept_, &name_,
                         &random_generate_}),
      ui_shared_resources_(ui_shared_resources),
      random_generator_(std::random_device{}()),
      base_instances_(base_instances),
      selected_id_(selected_id),
      value_config_(ui_shared_resources, text_renderer,
                    {{data::VboIdMain::kObjectsEditCharacteristicArea},
                     {data::VboIdMain::kObjectsEditCharacteristicIcon}},
                    {text_renderer,
                     {data::VboIdMain::kObjectsEditModelName},
                     data::TextId::kPerlin}),
      mdl_manager_(mdl_manager) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &accept_, &name_, &random_generate_);
  value_config_.AttachToHierarchy(hierarchy_);
  speed_ = 2.0f;
}

UiEditObjects::UiEditObjects(UiEditObjects&& other) noexcept
    : Base(std::move(other)),
      accept_(std::move(other.accept_)),
      name_(std::move(other.name_)),
      random_generate_(std::move(other.random_generate_)),
      ui_event_handler_({&pin_, &value_config_.sl_strength_, &accept_, &name_,
                         &random_generate_}),
      ui_shared_resources_(other.ui_shared_resources_),
      instances_(std::move(other.instances_)),
      base_instances_(other.base_instances_),
      selected_id_(other.selected_id_),
      random_generator_(other.random_generator_),
      value_config_(std::move(other.value_config_)),
      mdl_manager_(other.mdl_manager_) {
  hierarchy_ =
      UiHierarchy(&sprite_, &pin_, &accept_, &name_, &random_generate_);
  value_config_.AttachToHierarchy(hierarchy_);
}

void UiEditObjects::HideAll() { ForceHide(); }

void UiEditObjects::CreateInstance() { instances_.push_back(ObjectTraits{}); }

void UiEditObjects::UpdateConfig() {}

void UiEditObjects::SetInstanceId(int id) {}

bool UiEditObjects::Press(int id) {
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

void UiEditObjects::Release() {
  value_config_.pressed_strength_id_ = -1;
  value_config_.sl_strength_.Release();
  ui_event_handler_.Release();
}

bool UiEditObjects::Scroll(GLuint id, float yoffset) { return false; }

bool UiEditObjects::Render() {
  ui_shared_resources_.tex_ui_.Bind();
  auto mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_;
  bool stop_show = Base::RenderBack(true);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  accept_.Render();

  random_generate_.Render();
  name_.RenderBack();

  RenderModelConfig();

  // text section

  base_instances_[selected_id_].name = name_.GetText();
  name_.RenderText();

  return false;
}

void UiEditObjects::RenderPicking() {
  RenderPickingBack();
  ui_shared_resources_.shader_sp_picking_.Bind();

  accept_.RenderPicking();
  name_.RenderPicking();
  random_generate_.RenderPicking();

  RenderPickingModelConfig();

  ui_shared_resources_.shader_sp_picking_.Bind();
  name_.RenderPicking();
}

void UiEditObjects::Generate() {
  const auto& model = mdl_manager_.GetLoadedModels()[selected_id_];
  model->hp = instances_[selected_id_].hp;
  model->speed = instances_[selected_id_].speed;
  model->attack = instances_[selected_id_].attack;
  model->attack_speed = instances_[selected_id_].attack_speed;
  std::cout << "Objects characteristics updated:" << ' '
            << instances_[selected_id_].hp << ' '
            << instances_[selected_id_].speed << ' '
            << instances_[selected_id_].attack << ' '
            << instances_[selected_id_].attack_speed << std::endl;
}

void UiEditObjects::RandomGenerate() {
  std::uniform_real_distribution<float> dist_float(0.0f, 1.0f);
  std::bernoulli_distribution dist_bool(0.5f);
  instances_[selected_id_].hp = dist_float(random_generator_);
  instances_[selected_id_].speed = dist_float(random_generator_);
  instances_[selected_id_].attack = dist_float(random_generator_);
  instances_[selected_id_].attack_speed = dist_float(random_generator_);
  Generate();
}

ObjectTraits& UiEditObjects::GetInstanceData() noexcept {
  return instances_[selected_id_];
}

void UiEditObjects::RenderGraph() {
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

void UiEditObjects::Reset() { instances_.clear(); }

void UiEditObjects::MergeLayers() {}  // TODO:

void UiEditObjects::RenderModelConfig() {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = sprite_.GetHeight() / (4 + 5);  // +pads
  value_config_.Render(instances_[selected_id_].hp, next_offset,
                       value_config_.pressed_strength_id_ == 0,
                       data::TextId::kStrength);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].speed, next_offset,
                       value_config_.pressed_strength_id_ == 1,
                       data::TextId::kSwell);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].attack, next_offset,
                       value_config_.pressed_strength_id_ == 2,
                       data::TextId::kSwell);
  next_offset.y -= entry_height;
  value_config_.Render(instances_[selected_id_].attack_speed, next_offset,
                       value_config_.pressed_strength_id_ == 3,
                       data::TextId::kSwell);
}

void UiEditObjects::RenderPickingModelConfig() {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = sprite_.GetHeight() / (4 + 5);  // +pads
  value_config_.RenderPicking(next_offset);
  next_offset.y -= entry_height;
  value_config_.RenderPicking(next_offset);
  next_offset.y -= entry_height;
}
