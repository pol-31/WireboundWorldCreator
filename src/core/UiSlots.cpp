#include "UiSlots.h"

#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../common/OpenGlUtility.h"

/// parent is back_ BUT UiSlots is taken from slider, so
/// outside is's shown as a slider area
UiSlots::UiSlots(
    Tile& cur_tile,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : UiBase(static_cast<int>(data::VboIdMain::kTerrainSlotsBack), {}),
      ui_shared_resources_(ui_shared_resources),
      sl_data_({{data::VboIdMain::kTerrainSlotsSlider},
                {data::VboIdMain::kTerrainSlotsHandler},
                6, 0.75f, 0.8f}),
      back_(data::VboIdMain::kTerrainSlotsBack),
      create_(data::VboIdMain::kTerrainSlotsCreate,
              [this]() {
                this->CreateGraph();
                SelectGraph(instances_.size() - 1);
                sl_data_.SetEntryNum(instances_.size());
                sl_data_.Set(1.0f);
              }),
      slot_name_(text_renderer, data::VboIdMain::kTerrainSlotsName),
      slot_config_(data::VboIdMain::kTerrainSlotsConfig,
                   [this]() {
                     SelectGraph(GetSlotId());
                     ui_edit_->Show();
                   }),
      toggle_slot_visible_(
          {data::VboIdMain::kTerrainVisibleOff,
           [this]() {
//             auto slot_id = GetSlotId();
//             instances_[slot_id].do_show = !instances_[slot_id].do_show;
//             ui_edit_.UpdateHmap();
           }},
          {data::VboIdMain::kTerrainVisibleOn1},
          {data::VboIdMain::kTerrainVisibleOn2},
          {data::VboIdMain::kTerrainVisibleOn3}),
      slot_back_(data::VboIdMain::kTerrainSlotsSlot,
                 [this]() {
                   auto cursor_slot_id = GetSlotId();
                   if (selected_slot_id_ == cursor_slot_id) {

                     selected_slot_id_ = -1;
                   } else {
                     SelectGraph(cursor_slot_id);
                   }
                 }),
      slot_color_(data::VboIdMain::kTerrainSlotsSlotColor),
      slot_remove_(data::VboIdMain::kTerrainSlotsRemove,
                   [this]() {
                     auto slot_id = GetSlotId();
                     std::cout << "graph removed " << slot_id << std::endl;
                     if (slot_id == GetSelectedSlotId()) {
                       ui_edit_->HideAll();
                     }
                     RemoveGraph(slot_id);
                     ui_edit_->UpdateConfig();
                     sl_data_.UpdateRenderData(
                         /*slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
                         graph_.GetSize()*/);
                   }),
      slot_selected_(data::VboIdMain::kTerrainSlotsSelected),
//      ui_event_handler_({&create_, &slot_config_,
//                         &toggle_slot_visible_, &slot_back_, &slot_remove_}),
      hierarchy_(&back_) {
  hierarchy_ = UiHierarchy(
      &back_, &create_, &sl_data_);
  hierarchy_.AddNested(
      &slot_back_, &slot_name_, &slot_config_, &toggle_slot_visible_,
      &slot_color_, &slot_remove_, &slot_selected_);
//  gUiComponents[hierarchy_.parent_->GetId() - details::kIdOffsetUi].ui =
//      static_cast<UiBase*>(this);
//  IUiSlots::ui_event_handler_ = &ui_event_handler_;
//  IUiSlots::ui_edit_ = &ui_edit_;
  sl_data_.SetSlotPtr(&slot_back_);
}

UiSlots::UiSlots(UiSlots&& other) noexcept
    : UiBase(std::move(other)),
      sl_data_(std::move(other.sl_data_)),
//      handler_(std::move(other.handler_)),
//      slider_(std::move(other.slider_)),
      back_(std::move(other.back_)),
      create_(std::move(other.create_)),
      slot_name_(std::move(other.slot_name_)),
      slot_config_(std::move(other.slot_config_)),
      toggle_slot_visible_(std::move(other.toggle_slot_visible_)),
      slot_back_(std::move(other.slot_back_)),
      slot_color_(std::move(other.slot_color_)),
      slot_remove_(std::move(other.slot_remove_)),
      slot_selected_(std::move(other.slot_selected_)),
      ui_edit_(other.ui_edit_),
      selected_slot_id_(other.selected_slot_id_),
//      ui_event_handler_({&create_, &slot_config_,
//                         &toggle_slot_visible_, &slot_back_, &slot_remove_}),
      ui_shared_resources_(other.ui_shared_resources_),
      hierarchy_(std::move(other.hierarchy_)) {
  hierarchy_ = UiHierarchy(
      &back_, /*&handler_, &slider_, */&create_, &sl_data_);
  hierarchy_.AddNested(
      &slot_back_, &slot_name_, &slot_config_, &toggle_slot_visible_,
      &slot_color_, &slot_remove_, &slot_selected_);
//  IUiSlots::ui_event_handler_ = &ui_event_handler_;
  //  IUiSlots::ui_edit_ = &ui_edit_;
}

void UiSlots::AddInstance(BaseInstanceData&& data) {
  if (instances_.size() >= gMaxLayers) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    ui_edit_->CreateInstance();
    instances_.push_back(data);
  }
  sl_data_.SetEntryNum(instances_.size());
  sl_data_.Set(0.0f);
}

bool UiSlots::Press(int pressed_id) {
  if (pressed_id == create_.GetId()) {
    create_.Press();
  } else if (pressed_id == slot_config_.GetId()) {
    slot_config_.Press();
  } else if (pressed_id == toggle_slot_visible_.GetId()) {
    toggle_slot_visible_.Press();
  } else if (pressed_id == slot_back_.GetId()) {
    slot_back_.Press();
  } else if (pressed_id == slot_remove_.GetId()) {
    slot_remove_.Press();
  } else {
    return false;
  }
  return true;
}

void UiSlots::Release() {
  // no components really need it
}

void UiSlots::RenderSlotsSprites() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_, static_cast<int>(instances_.size()));
  glm::vec4 color_reset = glm::vec4{1.0f};
  for (int i = 0; i < slots_to_render; ++i) {
    auto graph_base_data = &instances_[i + sl_data_.cur_slots_offset_];
    slot_back_.SetTranslate(next_offset);
    slot_remove_.SetTranslate(next_offset);
    slot_config_.SetTranslate(next_offset);
    toggle_slot_visible_.SetTranslate(next_offset);
    slot_color_.SetTranslate(next_offset);

    slot_back_.Render();
    slot_remove_.Render();
    slot_config_.Render();
    toggle_slot_visible_.Set(!graph_base_data->do_show);
    toggle_slot_visible_.Render();

    glUniform4fv(7, 1, glm::value_ptr(graph_base_data->color));
    slot_color_.Render();
    glUniform4fv(7, 1, glm::value_ptr(color_reset));

    if (GetSelectedSlotId() - sl_data_.cur_slots_offset_ == i) {
      slot_selected_.SetTranslate(next_offset);
      slot_selected_.Render();
    }

    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiSlots::RenderSlotsText() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_, static_cast<int>(instances_.size()));
  for (int i = 0; i < slots_to_render; ++i) {
    auto graph_name = &instances_[i + sl_data_.cur_slots_offset_].name;
    slot_name_.Render(*graph_name, 1.0f, next_offset);
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiSlots::Render(glm::vec2 mouse_pos) { // done
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  back_.Render();
  sl_data_.Render(mouse_pos);
  sl_data_.SetUpScissors();
  glEnable(GL_SCISSOR_TEST);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  RenderSlotsSprites();
  RenderSlotsText();
  glDisable(GL_SCISSOR_TEST);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  create_.Render();
}

void UiSlots::RenderPickingSlotsSprites() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_, static_cast<int>(instances_.size()));
  for (int i = 0; i < slots_to_render; ++i) {
    slot_back_.SetTranslate(next_offset);
    slot_remove_.SetTranslate(next_offset);
    slot_config_.SetTranslate(next_offset);
    toggle_slot_visible_.SetTranslate(next_offset);
    slot_color_.SetTranslate(next_offset);

    slot_back_.RenderPicking();
    slot_remove_.RenderPicking();
    slot_config_.RenderPicking();
    toggle_slot_visible_.RenderPicking();
    slot_color_.RenderPicking();

    if (debug::gUiAltMode && (GetSelectedSlotId() - sl_data_.cur_slots_offset_) == i) {
      slot_selected_.SetTranslate(next_offset);
      slot_selected_.RenderPicking();
    }
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiSlots::RenderPickingSlotsText() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_, static_cast<int>(instances_.size()));
  for (int i = 0; i < slots_to_render; ++i) {
    auto graph_name = &instances_[i + sl_data_.cur_slots_offset_].name;
    slot_name_.RenderPicking(*graph_name, 1.0f, next_offset);
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiSlots::RenderPicking() { // done
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  back_.RenderPicking();
  sl_data_.RenderPicking();
  sl_data_.SetUpScissors();
  glEnable(GL_SCISSOR_TEST);
  RenderPickingSlotsSprites();
  RenderPickingSlotsText();
  glDisable(GL_SCISSOR_TEST);
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  create_.RenderPicking();
}

void UiSlots::CreateGraph() {
  if (instances_.size() >= gMaxLayers) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    ui_edit_->CreateInstance();
    instances_.emplace_back();
  }
}

void UiSlots::SelectGraph(GLuint id) {
  if (id >= instances_.size()) {
    throw "select non-existent slot id";
  }
  selected_slot_id_ = id;
  sl_data_.FocusOnSelected(id);
  ui_edit_->SetInstanceId(id);
}

void UiSlots::RemoveGraph(GLuint id) {
  if (id >= instances_.size()) {
    throw "remove non-existent graph id";
  }
  if (selected_slot_id_ > id) {
    --selected_slot_id_;
  } else if (selected_slot_id_ == id) {
    selected_slot_id_ = -1;
  }
  instances_.erase(instances_.begin() + id);
  std::cout << "*slot removed " << id << std::endl;
}

int UiSlots::GetSize() const noexcept {
  return instances_.size();
}

void UiSlots::UpdateTransform() {
  /*sl_data_.length_ = sl_data_.track_length_factor *
                     (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.length_slots_ = sl_data_.slots_length_factor *
                           (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.centre_ = (back_.GetTopBorder() + back_.GetBottomBorder()) / 2.0f;
  float related_pos = sl_data_.progress_ * sl_data_.length_ - sl_data_.length_ / 2.0f + sl_data_.centre_;
  sl_data_.Set({0.0f, related_pos}, handler_,
               slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
               graph_.GetSize());
  hierarchy_.UpdateTransform();*/
}

bool UiSlots::Scroll(GLuint id, float yoffset) {
  /*if (!ui_event_handler_.IsInRange(id)) {
//  if (id < slot_back_.GetId()) {
    return false;
  }
  float normalized_yoffset = 0.2f * yoffset;
  float factor = 1.0f / std::max(graph_.GetSize() - sl_data_.slots_num, 1);
  float progress = std::clamp(sl_data_.progress_ - normalized_yoffset * factor, 0.0f, 1.0f);
  sl_data_.Set(progress, handler_,
               slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
               graph_.GetSize());
  return true;*/
  return false;
}

int UiSlots::GetSlotId() {
  return sl_data_.GetSlotId(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
}

int UiSlots::GetSelectedSlotId() {
  return selected_slot_id_;
}

void UiSlots::Reset() {
  sl_data_.SetEntryNum(0);
  ui_edit_->ForceHide();
  instances_.clear(); // for some reason doesn't work
  selected_slot_id_ = -1;
}

BaseInstanceData* UiSlots::GetInstanceBaseData() {
  return &instances_[selected_slot_id_];
}
