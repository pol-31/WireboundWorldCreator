#include "UiSlots.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../common/OpenGlUtility.h"
#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../modes/UiEdit.h"

/// parent is back_ BUT UiSlots is taken from slider, so
/// outside is's shown as a slider area
UiSlots::UiSlots(UiRenderData& render_data,
                 TextRenderer& text_renderer,
                 std::function<void()> on_selection)
    : UiBase(static_cast<int>(data::VboIdMain::kSlotsBack), {}),
      on_selection_(on_selection),
      render_data_(render_data),
      sl_data_({data::VboIdMain::kSlotsSlider},
               {data::VboIdMain::kSlotsHandler}, 6, 0.75f, 0.8f),
      back_(data::VboIdMain::kSlotsBack),
      create_(data::VboIdMain::kSlotsCreate, [this]() { this->CreateGraph(); }),
      slot_name_(text_renderer, data::VboIdMain::kSlotsName),
      slot_config_(data::VboIdMain::kSlotsConfig,
                   [this]() {
                     SelectGraph(GetHoveredSlotId());
                     ui_edit_->Show();
                   }),
      toggle_slot_visible_({data::VboIdMain::kSlotsVisibleOff,
                            [this]() {
                              auto slot_id = GetHoveredSlotId();
                              auto instance =
                                  ui_edit_->GetBaseInstanceData(slot_id);
                              instance->do_show = !instance->do_show;
                              ui_edit_->UpdateConfig();
                            }},
                           {data::VboIdMain::kSlotsVisibleOn1},
                           {data::VboIdMain::kSlotsVisibleOn2},
                           {data::VboIdMain::kSlotsVisibleOn3}),
      slot_back_(data::VboIdMain::kSlotsSlot,
                 [this]() {
                   auto cursor_slot_id = GetHoveredSlotId();
                   if (*selected_id_ == cursor_slot_id) {
                     *selected_id_ = -1;
                     ui_edit_->UpdateConfig();
                     on_selection_();
                     return;
                   }
                   SelectGraph(cursor_slot_id);
                 }),
      slot_color_(data::VboIdMain::kSlotsSlotColor),
      slot_remove_(data::VboIdMain::kSlotsRemove,
                   [this]() {
                     auto slot_id = GetHoveredSlotId();
                     std::cout << "graph removed " << slot_id << std::endl;
                     if (slot_id == GetSelectedSlotId()) {
                       ui_edit_->HideAll();
                     }
                     RemoveGraph(slot_id);
                   }),
      slot_selected_(data::VboIdMain::kSlotsSelected),
      ui_event_handler_({&create_, &slot_config_, &toggle_slot_visible_,
                         &slot_back_, &slot_remove_}),
      hierarchy_(&back_, {&create_, &sl_data_}) {
  UiBase* comps[] = {&slot_name_,  &slot_config_, &toggle_slot_visible_,
                     &slot_color_, &slot_remove_, &slot_selected_};
  hierarchy_.AddNested(&slot_back_, comps);
  //  gUiComponents[hierarchy_.parent_->GetId() - details::kIdOffsetUi].ui =
  //      static_cast<UiBase*>(this);
  sl_data_.SetSlotPtr(&slot_back_);
  SetupUiHierarchy();
}

void UiSlots::Setup(IUiEdit* ui_edit, std::function<void()> on_selection) {
  if (ui_edit) {
    ui_edit->HideAll();
  }
  ui_edit_ = ui_edit;
  ui_edit_->SetUp();
  sl_data_.SetEntryNum(GetSize());
  selected_id_ = ui_edit->GetSelectedIdPtr();
  on_selection_ = std::move(on_selection);
  on_selection_();
}

void UiSlots::AddInstance(BaseInstanceData&& data) {
  if (GetSize() >= gMaxLayers) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    ui_edit_->CreateInstance();
  }
  sl_data_.SetEntryNum(GetSize());
}

bool UiSlots::Press(int pressed_id) {
  return ui_event_handler_.Press(pressed_id);
}

void UiSlots::Release() {
  // TODO: no components really need it
  ui_event_handler_.Release();
}

void UiSlots::RenderSlotsSprites() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(sl_data_.slots_num_, GetSize());
  glm::vec4 color_reset = glm::vec4{1.0f};
  for (int i = 0; i < slots_to_render; ++i) {
    auto graph_base_data =
        ui_edit_->GetBaseInstanceData(i + sl_data_.cur_slots_offset_);
    slot_back_.SetTranslate(next_offset);
    slot_remove_.SetTranslate(next_offset);
    slot_config_.SetTranslate(next_offset);
    toggle_slot_visible_.SetTranslate(next_offset);
    slot_color_.SetTranslate(next_offset);

    slot_back_.Render();
    slot_remove_.Render();
    slot_config_.Render();
    toggle_slot_visible_.Render(graph_base_data->do_show);

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
  int slots_to_render = std::min(sl_data_.slots_num_, GetSize());
  for (int i = 0; i < slots_to_render; ++i) {
    const auto& graph_name =
        ui_edit_->GetBaseInstanceData(i + sl_data_.cur_slots_offset_)->name;
    slot_name_.Render(graph_name, 1.0f, next_offset,
                      TextRenderer::Alignment::kCentre);
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiSlots::Render() {
  auto mouse_pos = render_data_.glfw_context_.cursor_pos_tex_norm_;
  render_data_.shader_sp_.Bind();
  render_data_.tex_ui_.BindSampler(0);
  back_.Render();
  sl_data_.Render(mouse_pos);
  sl_data_.SetUpScissors();
  glEnable(GL_SCISSOR_TEST);
  render_data_.shader_sp_.Bind();
  render_data_.tex_ui_.BindSampler(0);
  RenderSlotsSprites();
  RenderSlotsText();
  glDisable(GL_SCISSOR_TEST);
  render_data_.shader_sp_.Bind();
  render_data_.tex_ui_.BindSampler(0);
  create_.Render();
}

void UiSlots::RenderPickingSlotsSprites() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(sl_data_.slots_num_, GetSize());
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

    if (debug::gUiAltMode &&
        (GetSelectedSlotId() - sl_data_.cur_slots_offset_) == i) {
      slot_selected_.SetTranslate(next_offset);
      slot_selected_.RenderPicking();
    }
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiSlots::RenderPickingSlotsText() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(sl_data_.slots_num_, GetSize());
  for (int i = 0; i < slots_to_render; ++i) {
    const auto& graph_name =
        ui_edit_->GetBaseInstanceData(i + sl_data_.cur_slots_offset_)->name;
    slot_name_.RenderPicking(graph_name, 1.0f, next_offset,
                             TextRenderer::Alignment::kCentre);
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiSlots::RenderPicking() {  // done
  render_data_.shader_sp_picking_.Bind();
  render_data_.tex_ui_.BindSampler(0);
  back_.RenderPicking();
  sl_data_.RenderPicking();
  sl_data_.SetUpScissors();
  glEnable(GL_SCISSOR_TEST);
  RenderPickingSlotsSprites();
  RenderPickingSlotsText();
  glDisable(GL_SCISSOR_TEST);
  render_data_.shader_sp_picking_.Bind();
  render_data_.tex_ui_.BindSampler(0);
  create_.RenderPicking();
}

void UiSlots::CreateGraph() {
  if (GetSize() >= gMaxLayers) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    ui_edit_->CreateInstance();
    SelectGraph(GetSize() - 1);
    sl_data_.SetEntryNum(GetSize());
  }
}

void UiSlots::SelectGraph(GLuint id) {
  if (id >= GetSize()) {
    throw "select non-existent slot id";
  }
  *selected_id_ = static_cast<int>(id);
  sl_data_.FocusOnSelected(id);
  ui_edit_->SetInstanceId(id);
  on_selection_();
}

void UiSlots::RemoveGraph(GLuint id) {
  if (id >= GetSize()) {
    throw "remove non-existent graph id";
  }
  auto int_id = static_cast<int>(id);
  if (*selected_id_ > int_id) {
    --selected_id_;
  } else if (*selected_id_ == int_id) {
    *selected_id_ = -1;
  }
  ui_edit_->RemoveInstance(id);
  sl_data_.SetEntryNum(GetSize());
  on_selection_();
}

int UiSlots::GetSize() const noexcept { return ui_edit_->GetInstancesNum(); }

void UiSlots::UpdateTransform() {
  /*sl_data_.length_ = sl_data_.track_length_factor *
                     (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.length_slots_ = sl_data_.slots_length_factor *
                           (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.centre_ = (back_.GetTopBorder() + back_.GetBottomBorder()) / 2.0f;
  float related_pos = sl_data_.progress_ * sl_data_.length_ - sl_data_.length_
  / 2.0f + sl_data_.centre_; sl_data_.Set({0.0f, related_pos}, handler_,
               slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
               graph_.GetSize());*/
  hierarchy_.UpdateTransform();
}

bool UiSlots::Scroll(GLuint id, float yoffset) {
  /*if (!ui_event_handler_.IsInRange(id)) {
//  if (id < slot_back_.GetId()) {
    return false;
  }
  float normalized_yoffset = 0.2f * yoffset;
  float factor = 1.0f / std::max(graph_.GetSize() - sl_data_.slots_num, 1);
  float progress = std::clamp(sl_data_.progress_ - normalized_yoffset * factor,
0.0f, 1.0f); sl_data_.Set(progress, handler_, slot_back_.GetTopBorder() -
slot_back_.GetBottomBorder(), graph_.GetSize()); return true;*/
  return false;
}

int UiSlots::GetHoveredSlotId() {
  return sl_data_.GetSlotId(
      render_data_.glfw_context_.cursor_pos_tex_norm_);
}

int UiSlots::GetSelectedSlotId() const { return *selected_id_; }

void UiSlots::DeSelect() {
  ui_edit_->HideAll();
  *selected_id_ = -1;
  on_selection_();
}

void UiSlots::Reset() {
  sl_data_.SetEntryNum(0);
  ui_edit_->HideAll();
  *selected_id_ = -1;
  on_selection_();
}

BaseInstanceData* UiSlots::GetInstanceBaseData() {
  return ui_edit_->GetBaseInstanceData(*selected_id_);
}

void UiSlots::SetupUiHierarchy() {
  LocalTransform transform;
  transform.translate.x = 0.447f * back_.GetWidth();
  transform.translate.y = 0.03f * back_.GetHeight();
  sl_data_.SetParentTransform(transform);
  transform.translate.x = 0.0062f;
  transform.translate.y = 0.28975f * back_.GetHeight();
  slot_back_.SetParentTransform(transform);
}
