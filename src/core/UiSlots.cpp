#include "UiSlots.h"

#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"
#include "../core/TileRenderer.h"

const float UiSlotsSliderData::kTrackLengthFactor = 0.75f;
const float UiSlotsSliderData::kSlotsLengthFactor = 0.8f;
const int UiSlotsSliderData::kSlotsNum = 6;

void UiSlotsSliderData::UpdateRenderData(float track_length, int graphs_num) {
  //    auto graphs_num = graph_.GetSize();
  // total 6, visible 5
  slot_height_ = kSlotsLengthFactor * track_length;
  auto scrollable_slots = static_cast<float>(std::max(graphs_num - 5, 0));
  float float_index = scrollable_slots * progress_;
  cur_slots_offset_ = (int)float_index;
  float fractional_part = float_index - cur_slots_offset_;
  float offset_y = fractional_part * slot_height_;
  start_slot_translate_ = glm::vec2{0.0f, -0.201f + offset_y};

  // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
  float y_ndc = centre_ - length_ / 2;
  scissors_start_ = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
  scissors_length_ = int(length_ * 0.5f * gWindowHeight);
}

//    auto mouse_pos =
//        ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
int UiSlotsSliderData::GetSlotId(glm::vec2 mouse_pos) {
  float half_slot_height = slot_height_ / 2.0f;
  float border = centre_ + length_slots_ / 2.0f + half_slot_height + start_slot_translate_.y;
  for (int i = 0; i < 5; ++i) {
    if (mouse_pos.y > border) {
      return i + cur_slots_offset_;
    }
    border -= slot_height_;
  }
  return 5 + cur_slots_offset_; // else cond
}

void UiSlotsSliderData::Set(
    glm::vec2 mouse_pos, UiDynamicSprite& handler_sprite,
    float track_length, int graphs_num) {
  float half_length_ = length_slots_ / 2.0f;
  float offset = glm::clamp(
      mouse_pos.y - centre_, -half_length_, +half_length_);
  progress_ = 1.0 - (offset + half_length_) / length_slots_;
  glm::vec2 translate = {0.0f, offset};
  handler_sprite.SetTranslate(translate);
  UpdateRenderData(track_length, graphs_num);
}

void UiSlotsSliderData::Set(
    float progress, UiDynamicSprite& handler_sprite,
    float track_length, int graphs_num) {
  // track length unscaled: need just top_border - bottom_border
  progress_ = progress;
  float half_length_ = length_slots_ / 2.0f;
  float offset = -((progress_ - 1.0f) * length_slots_) - half_length_;
  glm::vec2 translate = {0.0f, offset};
  handler_sprite.SetTranslate(translate);
  UpdateRenderData(track_length, graphs_num);
}

void UiSlotsSliderData::FocusOnSelected(
    int slot_id, int graphs_num,
    UiDynamicSprite& handler_sprite,
    const UiDynamicSprite& back_sprite) {
  if (slot_id == -1 || graphs_num < kSlotsNum) {
    return;
  }
  float fractional_part;
  if (slot_id - cur_slots_offset_ == 0) {
    fractional_part = 0.0f;
  } else if (slot_id - cur_slots_offset_ == 5) {
    fractional_part = 0.99f;
  } else {
    return;
  }
  float float_index = fractional_part + cur_slots_offset_;
  auto scrollable_slots = static_cast<float>(std::max(graphs_num - 5, 0));
  std::cout << "was " << progress_;
  progress_ = float_index / scrollable_slots;
  std::cout << " become " << progress_ << std::endl;
  Set(progress_, handler_sprite, graphs_num,
      back_sprite.GetTopBorder() - back_sprite.GetBottomBorder());
}

IUiSlots::IUiSlots(
    size_t vbo_texture_id, UiSharedResources& ui_shared_resources)
    : UiBase(vbo_texture_id, {}),
      ui_shared_resources_(ui_shared_resources) {}

void IUiSlots::Press() {
  sl_data_.pressed_ = true;
}

void IUiSlots::Release() {
  sl_data_.pressed_ = false;
  ui_event_handler_->Release();
  graph_->Release();
}

/// as a decorator for graph_

void IUiSlots::CreateGraph() {
  graph_->CreateGraph();
}

void IUiSlots::SelectGraph(GLuint id) {
  graph_->SelectGraph(id);
}

void IUiSlots::RemoveGraph(GLuint id) {
  graph_->RemoveGraph(id);
}

void IUiSlots::RenderGraph() {
  graph_->Render(ui_shared_resources_.global_glfw_callback_data_.cursor_pos_);
}

int IUiSlots::GetSize() const noexcept {
  return graph_->GetSize();
}

/// parent is back_ BUT UiSlots is taken from slider, so
/// outside is's shown as a slider area
UiSlotsModels::UiSlotsModels(
    UiSharedResources& ui_shared_resources,
    UiDynamicSprite&& handler,
    UiDynamicSprite&& slider,
    UiDynamicSprite&& back,
    UiDynamicSprite&& create,
    data::VboIdMain flip_select_edit_back_vbo_texture,
    data::VboIdMain flip_point_edge_back_vbo_texture,
    UiDynamicSprite&& flip_select_edit_sprite,
    UiDynamicSprite&& flip_point_edge_sprite,
    UiDynamicSprite&& slot_name,
    UiDynamicSprite&& slot_config,
    UiToggle&& toggle_slot_visible,
    UiDynamicSprite&& slot_back,
    UiDynamicSprite&& slot_color,
    UiDynamicSprite&& slot_remove,
    UiDynamicSprite&& slot_selected)
    : IUiSlots(back.GetId(), ui_shared_resources),
      handler_(std::move(handler)),
      slider_(std::move(slider)),
      back_(std::move(back)),
      create_(std::move(create)),
      flip_select_edit_back_(
          flip_select_edit_back_vbo_texture, [this]() {
            this->NextSelectMode();
          }),
      flip_point_edge_back_(
          flip_point_edge_back_vbo_texture, [this]() {
            this->NextClickMode();
          }),
      flip_select_edit_sprite_(std::move(flip_select_edit_sprite)),
      flip_point_edge_sprite_(std::move(flip_point_edge_sprite)),
      flip_select_edit_(
          flip_select_edit_sprite_,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f}, 1.0f, glm::pi<float>()}),
      flip_point_edge_(
          flip_point_edge_sprite_,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f}, 1.0f, glm::pi<float>()}),
      slot_name_(std::move(slot_name)),
      slot_config_(std::move(slot_config)),
      toggle_slot_visible_(std::move(toggle_slot_visible)),
      slot_back_(std::move(slot_back)),
      slot_color_(std::move(slot_color)),
      slot_remove_(std::move(slot_remove)),
      slot_selected_(std::move(slot_selected)),
      graph_(ui_shared_resources),
      ui_edit_(
          ui_shared_resources,
          {data::VboIdMain::kFencesEditDesk},
          {data::VboIdMain::kFencesEditAccept},
          {data::VboIdMain::kFencesEditName},
          {data::VboIdMain::kFencesEditNameBack},

          {{data::VboIdMain::kFencesColorPaletteHS},
           {data::VboIdMain::kFencesColorColorCursor},
           glm::vec2{1.0f}},
          {{data::VboIdMain::kFencesColorPaletteB},
           {data::VboIdMain::kFencesColorBrightnessCursor},
           1.0f},

          {data::VboIdMain::kFencesColorIndicator},
          {data::VboIdMain::kFencesTypeBack},
          {data::VboIdMain::kFencesTypeText},
          {data::VboIdMain::kFencesTypePrevious},
          {data::VboIdMain::kFencesTypeNext}),
      ui_event_handler_({&create_, &flip_select_edit_back_,
                         &flip_point_edge_back_}),
      ui_shared_resources_(ui_shared_resources) {
  /*hierarchy_ = UiHierarchy(
      &back_, &handler_, &slider_, &create_, &flip_select_edit_back_,
      &flip_point_edge_back_, &flip_select_edit_sprite_,
      &flip_point_edge_sprite_);
  hierarchy_.AddNested(
      &slot_back_, &slot_name_, &slot_config_, &toggle_slot_visible_,
      &slot_color_, &slot_remove_, &slot_selected_);*/
  sl_data_.length_ = sl_data_.kTrackLengthFactor
                     * (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.length_slots_ = sl_data_.kSlotsLengthFactor
                           * (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.centre_ = (back_.GetTopBorder() + back_.GetBottomBorder()) / 2.0f;
  IUiSlots::ui_event_handler_ = &ui_event_handler_;
  IUiSlots::graph_ = &graph_;
//  IUiSlots::ui_edit_ = &ui_edit_;
}

UiSlotsModels::UiSlotsModels(UiSlotsModels&& other) noexcept
    : IUiSlots(std::move(other)),
      handler_(std::move(other.handler_)),
      slider_(std::move(other.slider_)),
      back_(std::move(other.back_)),
      create_(std::move(other.create_)),
      flip_select_edit_back_(std::move(other.flip_select_edit_back_)),
      flip_point_edge_back_(std::move(other.flip_point_edge_back_)),
      flip_select_edit_sprite_(std::move(other.flip_select_edit_sprite_)),
      flip_point_edge_sprite_(std::move(other.flip_point_edge_sprite_)),
      flip_select_edit_(
          flip_select_edit_sprite_, other.flip_select_edit_.GetStart(),
          other.flip_select_edit_.GetEnd()),
      flip_point_edge_(
          flip_point_edge_sprite_, other.flip_point_edge_.GetStart(),
          other.flip_point_edge_.GetEnd()),
      slot_name_(std::move(other.slot_name_)),
      slot_config_(std::move(other.slot_config_)),
      toggle_slot_visible_(std::move(other.toggle_slot_visible_)),
      slot_back_(std::move(other.slot_back_)),
      slot_color_(std::move(other.slot_color_)),
      slot_remove_(std::move(other.slot_remove_)),
      slot_selected_(std::move(other.slot_selected_)),
      graph_(std::move(other.graph_)),
      ui_edit_(std::move(other.ui_edit_)),
      ui_event_handler_({&create_, &flip_select_edit_back_,
                         &flip_point_edge_back_}),
      ui_shared_resources_(other.ui_shared_resources_) {
  /*hierarchy_ = UiHierarchy(
      &back_, &handler_, &slider_, &create_, &flip_select_edit_back_,
      &flip_point_edge_back_, &flip_select_edit_sprite_,
      &flip_point_edge_sprite_, &slot_name_, &slot_config_,
      &toggle_slot_visible_, &slot_back_, &slot_color_);
  hierarchy_.AddNested(&slot_back_, &slot_remove_, &slot_selected_);*/
  IUiSlots::ui_event_handler_ = &ui_event_handler_;
  IUiSlots::graph_ = &graph_;
  //  IUiSlots::ui_edit_ = &ui_edit_;
}

void UiSlotsModels::Render(glm::vec2 mouse_pos) {
  if (sl_data_.pressed_) {
    sl_data_.Set(mouse_pos, handler_,
                 back_.GetTopBorder() - back_.GetBottomBorder(),
                 graph_.GetSize());
  }
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  back_.Render();
  slider_.Render();
  handler_.Render();
  glEnable(GL_SCISSOR_TEST);
  glScissor(0, sl_data_.scissors_start_, 4000, sl_data_.scissors_length_);

  bool show_selected = false;
  glm::vec2 selected_offset{0.0f};
  auto selected_slot_id = graph_.GetSlotId();
  auto graphs_num = graph_.GetSize();
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  for (int i = 0; i < std::min(sl_data_.kSlotsNum, graphs_num); ++i) {
    slot_back_.SetTranslate(next_offset);
    slot_remove_.SetTranslate(next_offset);
    slot_config_.SetTranslate(next_offset);
    toggle_slot_visible_.SetTranslate(next_offset);
    slot_color_.SetTranslate(next_offset);
    if (selected_slot_id - sl_data_.cur_slots_offset_ == i) {
      show_selected = true;
      selected_offset = next_offset;
    }
    slot_back_.Render();
    slot_remove_.Render();
    slot_config_.Render();
    toggle_slot_visible_.Render();
    slot_color_.Render();
    next_offset.y -= sl_data_.slot_height_;
  }
  next_offset = sl_data_.start_slot_translate_;
  for (int i = 0; i < std::min(sl_data_.kSlotsNum, graphs_num - sl_data_.cur_slots_offset_); ++i) {
    auto graph_name = graph_.GetNameRef(i + sl_data_.cur_slots_offset_);
    ui_shared_resources_.global_glfw_callback_data_.text_renderer->
        RenderText(slot_name_, *graph_name, 1.0f, next_offset);
    next_offset.y -= sl_data_.slot_height_;
  }

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  // we should draw it last (on top of slots)
  if (show_selected) {
    slot_selected_.SetTranslate(selected_offset);
    slot_selected_.Render();
  }
  glDisable(GL_SCISSOR_TEST);

  create_.Render();
  flip_select_edit_back_.Render();
  flip_point_edge_back_.Render();
  flip_select_edit_.Render();
  flip_point_edge_.Render();

//  ui_edit_.Render();
}

/* std::vector<GLuint> selected_vertices_;
 * std::vector<GLuint> selected_edges_; - by id, so RenderPicking select either
 *   kVertex \ kEdge \ kFace
 * std::vector<GLuint> selected_faces_; - similar to above - faceid
 *
 * ... then how to switch between them?
 * - vertex provides its own id as a terrain grid id
 * - edge should provide info {id, vertex_1, vertex_2}
 * - face should provide info {id, vertex_...} idk...
 *
 *
 *
 * */

void UiSlotsModels::UpdateTransform() {
  sl_data_.length_ = sl_data_.kTrackLengthFactor *
            (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.length_slots_ = sl_data_.kSlotsLengthFactor *
                  (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.centre_ = (back_.GetTopBorder() + back_.GetBottomBorder()) / 2.0f;
  float related_pos = sl_data_.progress_ * sl_data_.length_ - sl_data_.length_ / 2.0f + sl_data_.centre_;
  sl_data_.Set({0.0f, related_pos}, handler_,
               back_.GetTopBorder() - back_.GetBottomBorder(),
               graph_.GetSize());
}

void UiSlotsModels::RenderPicking() {
  back_.RenderPicking();
  slider_.RenderPicking();
  glEnable(GL_SCISSOR_TEST);
  glScissor(0, sl_data_.scissors_start_, 4000, sl_data_.scissors_length_);

  bool show_selected = false;
  glm::vec2 selected_offset{0.0f};
  auto selected_slot_id = graph_.GetSlotId();
  auto graphs_num = graph_.GetSize();
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  for (int i = 0; i < std::min(sl_data_.kSlotsNum, graphs_num); ++i) {
    slot_back_.SetTranslate(next_offset);
    slot_remove_.SetTranslate(next_offset);
    slot_config_.SetTranslate(next_offset);
    toggle_slot_visible_.SetTranslate(next_offset);
    slot_color_.SetTranslate(next_offset);
    if (debug::gUiAltMode && (selected_slot_id - sl_data_.cur_slots_offset_) == i) {
      show_selected = true;
      selected_offset = next_offset;
    }
    slot_back_.RenderPicking();
    slot_remove_.RenderPicking();
    slot_config_.RenderPicking();
    toggle_slot_visible_.RenderPicking();
    slot_color_.RenderPicking();
    next_offset.y -= sl_data_.slot_height_;
  }

  next_offset = sl_data_.start_slot_translate_;
  for (int i = 0; i < std::min(sl_data_.kSlotsNum, graphs_num - sl_data_.cur_slots_offset_); ++i) {
    auto graph_name = graph_.GetNameRef(i + sl_data_.cur_slots_offset_);
    ui_shared_resources_.global_glfw_callback_data_.text_renderer->
        RenderTextPicking(slot_name_, *graph_name, 1.0f, next_offset);
    next_offset.y -= sl_data_.slot_height_;
  }

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  // we should draw it last (on top of slots)
  if (debug::gUiAltMode && show_selected) {
    slot_selected_.SetTranslate(selected_offset);
    slot_selected_.RenderPicking();
  }
  glDisable(GL_SCISSOR_TEST);

  create_.RenderPicking();
  flip_select_edit_back_.RenderPicking();
  flip_point_edge_back_.RenderPicking();
  if (debug::gUiAltMode) {
    handler_.RenderPicking();
    //    flip_select_edit_.RenderPicking();
    //    flip_point_edge_.RenderPicking();
    flip_select_edit_.RenderPicking();
    flip_point_edge_.RenderPicking();
  }

//  ui_edit_.RenderPicking();
}

bool UiSlotsModels::Press(int id) {
  if (id == slot_back_.GetId()) {
    auto slot_id = sl_data_.GetSlotId(
        ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
    graph_.SelectGraph(slot_id);
    sl_data_.FocusOnSelected(slot_id, graph_.GetSize(), handler_, slot_back_);
  } else if (id == slot_remove_.GetId()) {
    auto removed_id = sl_data_.GetSlotId(
        ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
    if (graph_.GetSize() > 0) {
      std::cout << "graph removed " << removed_id << std::endl;
      graph_.RemoveGraph(removed_id);
      sl_data_.UpdateRenderData(slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
                                graph_.GetSize());
    }
  } else {
    bool handled = ui_event_handler_.Press(id);
//    if (!handled) {
//      handled = ui_edit_.Press(id);
//    }
    if (id == create_.GetId()) {
      // instead of FocusOnSelected();
      sl_data_.Set(1.0f, handler_,
                   back_.GetTopBorder() - back_.GetBottomBorder(),
                   graph_.GetSize());
    }
    return handled;
  }
  return true;
}

bool UiSlotsModels::Scroll(GLuint id, float yoffset) {
//  if (!ui_event_handler_.IsInRange(id)) {
  if (id < slot_back_.GetId() ||
    id > flip_select_edit_sprite_.GetId()) {
    return false;
  }
  float normalized_yoffset = 0.2f * yoffset;
  float factor = 1.0f / std::max(graph_.GetSize() - sl_data_.kSlotsNum, 1);
  float progress = std::clamp(sl_data_.progress_ - normalized_yoffset * factor, 0.0f, 1.0f);
  sl_data_.Set(progress, handler_,
               back_.GetTopBorder() - back_.GetBottomBorder(),
               graph_.GetSize());
  return true;
}

void UiSlotsModels::NextSelectMode() {
  switch (press_state_) {
    case PressState::kSelect:
      press_state_ = PressState::kModify;
      break;
    case PressState::kModify:
      press_state_ = PressState::kSelect;
      break;
  }
  flip_select_edit_.StopAnimation();
  switch (press_state_) {
    case PressState::kModify:
      flip_select_edit_.SetStart(
          {glm::vec2{0.0f}, 1.0f, 0.0f});
      flip_select_edit_.SetEnd(
          {glm::vec2{0.0f}, 1.0f, glm::pi<float>()});
      break;
    case PressState::kSelect:
      flip_select_edit_.SetStart(
          {glm::vec2{0.0f}, 1.0f, glm::pi<float>()});
      flip_select_edit_.SetEnd(
          {glm::vec2{0.0f}, 1.0f, glm::two_pi<float>()});
      break;
  }
  flip_select_edit_.RunAnimation();
}

void UiSlotsModels::NextClickMode() {
  switch (edit_state_) {
    case EditState::kVertices:
      edit_state_ = EditState::kEdges;
      break;
    case EditState::kEdges:
      edit_state_ = EditState::kFaces;
      break;
    case EditState::kFaces:
      edit_state_ = EditState::kVertices;
      break;
  }
  flip_point_edge_.StopAnimation();
  switch (edit_state_) {
    case EditState::kVertices:
      flip_point_edge_.SetStart(
          {glm::vec2{0.0f}, 1.0f, 0.0f});
      flip_point_edge_.SetEnd(
          {glm::vec2{0.0f}, 1.0f, glm::pi<float>() / 3.0f});
      break;
    case EditState::kEdges:
      flip_point_edge_.SetStart(
          {glm::vec2{0.0f}, 1.0f, glm::pi<float>() / 3.0f});
      flip_point_edge_.SetEnd(
          {glm::vec2{0.0f}, 1.0f, 2.0f * glm::pi<float>() / 3.0f});
      break;
    case EditState::kFaces:
      flip_point_edge_.SetStart(
          {glm::vec2{0.0f}, 1.0f, 2.0f * glm::pi<float>() / 3.0f});
      flip_point_edge_.SetEnd(
          {glm::vec2{0.0f}, 1.0f, glm::two_pi<float>()});
      break;
  }
  flip_point_edge_.RunAnimation();
}

void UiSlotsModels::PressGraph(GLuint id) {
  if (press_state_ == PressState::kSelect) {
    switch (edit_state_) {
      case EditState::kVertices:
        break;
      case EditState::kEdges:
        break;
      case EditState::kFaces:
        break;
    }
  } else { // PressState::kModify
    switch (edit_state_) {
      case EditState::kVertices:
        break;
      case EditState::kEdges:
        break;
      case EditState::kFaces:
        break;
    }
  }
}

// --- --- --- --- --- ---
// --- --- --- --- --- ---
// --- --- --- --- --- ---
// --- --- --- --- --- ---
// --- --- --- --- --- ---
// --- --- --- --- --- ---
// --- --- --- --- --- ---
// --- --- --- --- --- ---

/// parent is back_ BUT UiSlots is taken from slider, so
/// outside is's shown as a slider area
UiSlotsTerrain::UiSlotsTerrain(
    Tile& cur_tile,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer,
    const UiSliderV& slider_size,
    const UiSliderV& slider_falloff)
    : IUiSlots(static_cast<int>(data::VboIdMain::kTerrainSlotsBack),
               ui_shared_resources),
      handler_(data::VboIdMain::kTerrainSlotsHandler),
      slider_(data::VboIdMain::kTerrainSlotsSlider),
      back_(data::VboIdMain::kTerrainSlotsBack),
      create_(data::VboIdMain::kTerrainSlotsCreate,
              [this]() {
                this->CreateGraph();
                std::cout << "there input starts" << std::endl;
                std::cout << "-- slot create (create)" << std::endl;
                // instead of FocusOnSelected();
                SelectGraph(instances_size_ - 1);
                sl_data_.Set(1.0f, handler_,
                             slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
                             graph_.GetSize());
              }),
      flip_point_edge_back_(
          data::VboIdMain::kTerrainSlotsFlipPointEdgeFace_Back,
          [this]() {
            this->NextClickMode();
          }),
      flip_point_edge_sprite_(data::VboIdMain::kTerrainSlotsFlipPointEdgeFace),
      flip_point_edge_(
          flip_point_edge_sprite_,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.0f}, 1.0f, glm::pi<float>()}),
      slot_name_(text_renderer, data::VboIdMain::kTerrainSlotsName),
      slot_config_(data::VboIdMain::kTerrainSlotsConfig,
                   [this]() {
                     SelectGraph(GetSlotId());
                     ui_edit_.Show();
                   }),
      toggle_slot_visible_(
          {data::VboIdMain::kTerrainVisibleOff,
           [this]() {
             auto slot_id = GetSlotId();
             instances_[slot_id].do_show = !instances_[slot_id].do_show;
//             ui_edit_.UpdateHmap();
           }},
          {data::VboIdMain::kTerrainVisibleOn1},
          {data::VboIdMain::kTerrainVisibleOn2},
          {data::VboIdMain::kTerrainVisibleOn3}),
      slot_back_(data::VboIdMain::kTerrainSlotsSlot,
                 [this]() {
                   SelectGraph(GetSlotId());
                 }),
      slot_color_(data::VboIdMain::kTerrainSlotsSlotColor),
      slot_remove_(data::VboIdMain::kTerrainSlotsRemove,
                   [this]() {
                     auto slot_id = GetSlotId();
                     std::cout << "graph removed " << slot_id << std::endl;
                     if (slot_id == graph_.GetSlotId()) {
                       ui_edit_.HideAll();
                     }
                     graph_.RemoveGraph(slot_id);
                     ui_edit_.UpdateHmap();
                     sl_data_.UpdateRenderData(
                         slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
                         graph_.GetSize());
                   }),
      slot_selected_(data::VboIdMain::kTerrainSlotsSelected),
      graph_(ui_shared_resources, ui_edit_, instances_, instances_size_,
             slider_size, slider_falloff),
      ui_edit_(
          cur_tile,
          {data::VboIdMain::kTerrainEditDesk},
          1.0f,
          {{data::VboIdMain::kTerrainEditDeskPinBack, []() {}},
          {data::VboIdMain::kTerrainEditDeskPinPoint}},
          ui_shared_resources,
          window_queue,
          text_renderer,
          {data::VboIdMain::kTerrainEditAccept},
          {data::VboIdMain::kTerrainEditName},
          {data::VboIdMain::kTerrainEditNameBack},

          {{data::VboIdMain::kTerrainColorPaletteHS},
           {data::VboIdMain::kTerrainColorColorCursor},
           glm::vec2{1.0f}},
          {{data::VboIdMain::kTerrainColorPaletteB},
           {data::VboIdMain::kTerrainColorBrightnessCursor},
           1.0f},
          {data::VboIdMain::kTerrainColorIndicator},
          {data::VboIdMain::kTerrainEditRandomGeneration},
          instances_, instances_size_),
      ui_event_handler_({&create_, &flip_point_edge_back_, &slot_config_,
                         &toggle_slot_visible_, &slot_back_, &slot_remove_}),
      ui_shared_resources_(ui_shared_resources),
      hierarchy_(&back_) {
  hierarchy_ = UiHierarchy(
      &back_, &handler_, &slider_, &create_, &flip_point_edge_back_,
      &flip_point_edge_sprite_);
  hierarchy_.AddNested(
      &slot_back_, &slot_name_, &slot_config_, &toggle_slot_visible_,
      &slot_color_, &slot_remove_, &slot_selected_);
  gUiComponents[hierarchy_.parent_->GetId() - details::kIdOffsetUi].ui = this;

  sl_data_.length_ = sl_data_.kTrackLengthFactor
                     * (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.length_slots_ = sl_data_.kSlotsLengthFactor
                           * (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.centre_ = (back_.GetTopBorder() + back_.GetBottomBorder()) / 2.0f;

  IUiSlots::ui_event_handler_ = &ui_event_handler_;
  IUiSlots::graph_ = &graph_;
//  IUiSlots::ui_edit_ = &ui_edit_;
}

UiSlotsTerrain::UiSlotsTerrain(UiSlotsTerrain&& other) noexcept
    : IUiSlots(std::move(other)),
      handler_(std::move(other.handler_)),
      slider_(std::move(other.slider_)),
      back_(std::move(other.back_)),
      create_(std::move(other.create_)),
      flip_point_edge_back_(std::move(other.flip_point_edge_back_)),
      flip_point_edge_sprite_(std::move(other.flip_point_edge_sprite_)),
      flip_point_edge_(
          flip_point_edge_sprite_, other.flip_point_edge_.GetStart(),
          other.flip_point_edge_.GetEnd()),
      slot_name_(std::move(other.slot_name_)),
      slot_config_(std::move(other.slot_config_)),
      toggle_slot_visible_(std::move(other.toggle_slot_visible_)),
      slot_back_(std::move(other.slot_back_)),
      slot_color_(std::move(other.slot_color_)),
      slot_remove_(std::move(other.slot_remove_)),
      slot_selected_(std::move(other.slot_selected_)),
      graph_(std::move(other.graph_)),
      ui_edit_(std::move(other.ui_edit_)),
      ui_event_handler_({&create_, &flip_point_edge_back_, &slot_config_,
                         &toggle_slot_visible_, &slot_back_, &slot_remove_}),
      ui_shared_resources_(other.ui_shared_resources_),
      hierarchy_(std::move(other.hierarchy_)) {
  hierarchy_ = UiHierarchy(
      &back_, &handler_, &slider_, &create_, &flip_point_edge_back_,
      &flip_point_edge_sprite_);
  hierarchy_.AddNested(
      &slot_back_, &slot_name_, &slot_config_, &toggle_slot_visible_,
      &slot_color_, &slot_remove_, &slot_selected_);
  IUiSlots::ui_event_handler_ = &ui_event_handler_;
  IUiSlots::graph_ = &graph_;
  //  IUiSlots::ui_edit_ = &ui_edit_;
}

void UiSlotsTerrain::Render(glm::vec2 mouse_pos) {
  if (sl_data_.pressed_) {
    sl_data_.Set(mouse_pos, handler_,
                 slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
                 graph_.GetSize());
  }
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  back_.Render();
  slider_.Render();
  handler_.Render();
  glEnable(GL_SCISSOR_TEST);
  glScissor(0, sl_data_.scissors_start_, 4000, sl_data_.scissors_length_);

  bool show_selected = false;
  glm::vec2 selected_offset{0.0f};
  auto selected_slot_id = graph_.GetSlotId();
  auto graphs_num = graph_.GetSize();
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  for (int i = 0; i < std::min(sl_data_.kSlotsNum, graphs_num); ++i) {
    slot_back_.SetTranslate(next_offset);
    slot_remove_.SetTranslate(next_offset);
    slot_config_.SetTranslate(next_offset);
    toggle_slot_visible_.SetTranslate(next_offset);
    slot_color_.SetTranslate(next_offset);
    if (selected_slot_id - sl_data_.cur_slots_offset_ == i) {
      show_selected = true;
      selected_offset = next_offset;
    }
    slot_back_.Render();
    slot_remove_.Render();
    slot_config_.Render();
    toggle_slot_visible_.Set(!instances_[sl_data_.cur_slots_offset_ + i].do_show);
    toggle_slot_visible_.Render();
    auto graph_base_data = graph_.GetBaseInstanceData(i + sl_data_.cur_slots_offset_);
    glUniform4fv(7, 1, glm::value_ptr(graph_base_data->color));
    slot_color_.Render();
    glm::vec4 color_white = glm::vec4{1.0f};
    glUniform4fv(7, 1, glm::value_ptr(color_white)); // restore
    next_offset.y -= sl_data_.slot_height_;
  }
  next_offset = sl_data_.start_slot_translate_;
  for (int i = 0; i < std::min(sl_data_.kSlotsNum, graphs_num - sl_data_.cur_slots_offset_); ++i) {
    auto graph_name = graph_.GetNameRef(i + sl_data_.cur_slots_offset_);
    slot_name_.Render(*graph_name, 1.0f, next_offset);
    next_offset.y -= sl_data_.slot_height_;
  }

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  // we should draw it last (on top of slots)
  if (show_selected) {
    slot_selected_.SetTranslate(selected_offset);
    slot_selected_.Render();
  }
  glDisable(GL_SCISSOR_TEST);

  create_.Render();
  flip_point_edge_back_.Render();
  flip_point_edge_.Render();

//  ui_shared_resources_.global_glfw_callback_data_.tile_renderer
//      ->terrain.Render(graph_.GetInstanceData());

//  glBindVertexArray(ui_shared_resources_.vao_ui_);
//  ui_edit_.Render();
}

void UiSlotsTerrain::RenderPicking() {
  back_.RenderPicking();
  slider_.RenderPicking();
  glEnable(GL_SCISSOR_TEST);
  glScissor(0, sl_data_.scissors_start_, 4000, sl_data_.scissors_length_);

  bool show_selected = false;
  glm::vec2 selected_offset{0.0f};
  auto selected_slot_id = graph_.GetSlotId();
  auto graphs_num = graph_.GetSize();
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  for (int i = 0; i < std::min(sl_data_.kSlotsNum, graphs_num); ++i) {
    slot_back_.SetTranslate(next_offset);
    slot_remove_.SetTranslate(next_offset);
    slot_config_.SetTranslate(next_offset);
    toggle_slot_visible_.SetTranslate(next_offset);
    slot_color_.SetTranslate(next_offset);
    if (debug::gUiAltMode && (selected_slot_id - sl_data_.cur_slots_offset_) == i) {
      show_selected = true;
      selected_offset = next_offset;
    }
    slot_back_.RenderPicking();
    slot_remove_.RenderPicking();
    slot_config_.RenderPicking();
    toggle_slot_visible_.RenderPicking();
    slot_color_.RenderPicking();
    next_offset.y -= sl_data_.slot_height_;
  }

  next_offset = sl_data_.start_slot_translate_;
  for (int i = 0; i < std::min(sl_data_.kSlotsNum, graphs_num - sl_data_.cur_slots_offset_); ++i) {
    auto graph_name = graph_.GetNameRef(i + sl_data_.cur_slots_offset_);
    slot_name_.RenderPicking(*graph_name, 1.0f, next_offset);
    next_offset.y -= sl_data_.slot_height_;
  }

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  // we should draw it last (on top of slots)
  if (debug::gUiAltMode && show_selected) {
    slot_selected_.SetTranslate(selected_offset);
    slot_selected_.RenderPicking();
  }
  glDisable(GL_SCISSOR_TEST);

  create_.RenderPicking();
  flip_point_edge_back_.RenderPicking();
  if (debug::gUiAltMode) {
    handler_.RenderPicking();
    //    flip_select_edit_.RenderPicking();
    //    flip_point_edge_.RenderPicking();
    flip_point_edge_.RenderPicking();
  }

  graph_.RenderPicking();

//  ui_shared_resources_.global_glfw_callback_data_.tile_renderer
//      ->terrain.RenderPicking(graph_.GetInstanceData());

//  glBindVertexArray(ui_shared_resources_.vao_ui_);
//  ui_edit_.RenderPicking();
}

void UiSlotsTerrain::UpdateTransform() {
  sl_data_.length_ = sl_data_.kTrackLengthFactor *
                     (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.length_slots_ = sl_data_.kSlotsLengthFactor *
                           (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.centre_ = (back_.GetTopBorder() + back_.GetBottomBorder()) / 2.0f;
  float related_pos = sl_data_.progress_ * sl_data_.length_ - sl_data_.length_ / 2.0f + sl_data_.centre_;
  sl_data_.Set({0.0f, related_pos}, handler_,
               slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
               graph_.GetSize());
  hierarchy_.UpdateTransform();
}

void UiSlotsTerrain::SelectGraph(GLuint id) {
/*  if (id == graph_.GetSlotId()) {
    graph_.SelectGraph(-1); // TODO: graph::DeSelect()
    ui_edit_.SetTerrainData(graph_.GetInstanceData());
    UpdateTransformUniform();
    ui_edit_.UpdateHmap(); // TODO: at some point we have terrain_data as nullptr
    return;
  }*/
  std::cout << "-- slot back (selected)" << std::endl;
  graph_.SelectGraph(id);
  sl_data_.FocusOnSelected(id, graph_.GetSize(), handler_, slot_back_);
  ui_edit_.SetTerrainData(graph_.GetInstanceData());
  UpdateTransformUniform();
}

bool UiSlotsTerrain::Press(int id) {
  std::cout << "Press()" << std::endl;
  if (id < details::kIdOffsetWater) {
    std::cout << "Add point #" << id << std::endl;
    std::cout << "Coordinates are: " << (id & 1023)
              << " and " << (id >> 10) << std::endl;
    //TODO: if water/other subtract maybe...
    bool shift_pressed = glfwGetKey(gWindow, GLFW_KEY_LEFT_SHIFT);
    bool ctrl_pressed = glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL);
    graph_.Press(ui_shared_resources_.global_glfw_callback_data_.cursor_pos_,
                 shift_pressed, ctrl_pressed);
    return false;
  }
  return ui_event_handler_.Press(id);
}

bool UiSlotsTerrain::Scroll(GLuint id, float yoffset) {
//  if (!ui_event_handler_.IsInRange(id)) {
  if (id < slot_back_.GetId() ||
    id > flip_point_edge_sprite_.GetId()) {
    return false;
  }
  float normalized_yoffset = 0.2f * yoffset;
  float factor = 1.0f / std::max(graph_.GetSize() - sl_data_.kSlotsNum, 1);
  float progress = std::clamp(sl_data_.progress_ - normalized_yoffset * factor, 0.0f, 1.0f);
  sl_data_.Set(progress, handler_,
               slot_back_.GetTopBorder() - slot_back_.GetBottomBorder(),
               graph_.GetSize());
  return true;
}

void UiSlotsTerrain::NextClickMode() {
  switch (edit_state_) {
    case EditState::kVertices:
      edit_state_ = EditState::kEdges;
      break;
    case EditState::kEdges:
      edit_state_ = EditState::kFaces;
      break;
    case EditState::kFaces:
      edit_state_ = EditState::kVertices;
      break;
  }
//  graph_.FlipSelectMode(); // TODO: temp
  flip_point_edge_.StopAnimation();
  switch (edit_state_) {
    case EditState::kVertices:
      flip_point_edge_.SetStart(
          {glm::vec2{0.0f}, 1.0f, 0.0f});
      flip_point_edge_.SetEnd(
          {glm::vec2{0.0f}, 1.0f, 2.0f * glm::pi<float>() / 3.0f});
      break;
    case EditState::kEdges:
      flip_point_edge_.SetStart(
          {glm::vec2{0.0f}, 1.0f, 2.0f * glm::pi<float>() / 3.0f});
      flip_point_edge_.SetEnd(
          {glm::vec2{0.0f}, 1.0f, 4.0f * glm::pi<float>() / 3.0f});
      break;
    case EditState::kFaces:
      flip_point_edge_.SetStart(
          {glm::vec2{0.0f}, 1.0f, 4.0f * glm::pi<float>() / 3.0f});
      flip_point_edge_.SetEnd(
          {glm::vec2{0.0f}, 1.0f, glm::two_pi<float>()});
      break;
  }
  flip_point_edge_.RunAnimation();
}

void UiSlotsTerrain::PressGraph(GLuint id) {
  switch (edit_state_) {
    case EditState::kVertices:
      break;
    case EditState::kEdges:
      break;
    case EditState::kFaces:
      break;
  }
}

//TODO: angles (input number), axis combinations, etc... (like in Blender)

void UiSlotsTerrain::TranslateSelected(glm::vec3 value) {
  int slot_id = graph_.GetSlotId();
  if (slot_id != -1) {
    auto prev_value = instances_[slot_id].translate;
    instances_[slot_id].translate = glm::clamp(
        prev_value + value * 100.0f, glm::vec3(-200.0f), glm::vec3(200.0f));
    UpdateTransformUniform();
  } else {
//    graph_.MoveSelected(value);
  }
}

void UiSlotsTerrain::RotateSelected(glm::vec3 value) {
  int slot_id = graph_.GetSlotId();
  if (slot_id != -1) {
    instances_[slot_id].rotate += value;
    UpdateTransformUniform();
  }
}

void UiSlotsTerrain::ScaleSelected(glm::vec3 value) {
  int slot_id = graph_.GetSlotId();
  if (slot_id != -1) {
    instances_[slot_id].scale += value;
    UpdateTransformUniform();
  }
}

void UiSlotsTerrain::UpdateTransformUniform() {
  int slot_id = graph_.GetSlotId();
  if (slot_id == -1) {
    return;
  }
  // skip, no effects
//  glm::mat4 model = {1.0f};
//  float map_scale = ui_shared_resources_.global_glfw_callback_data_
//                        .tile_renderer->cur_tile_.map_scale;
//  model = glm::scale(model, instances_[slot_id].scale * map_scale);
//  //TODO: use fast length (no square root)
//  model = glm::rotate(
//      model, glm::length(instances_[slot_id].rotate),
//      glm::normalize(instances_[slot_id].rotate));
//  model = glm::translate(model, instances_[slot_id].translate);
//  ui_shared_resources_.global_glfw_callback_data_.tile_renderer
//      ->terrain.UpdateTransformUniform(model);
}

int UiSlotsTerrain::GetSlotId() {
  return sl_data_.GetSlotId(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
}
