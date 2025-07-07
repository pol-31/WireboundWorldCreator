#include "UiSlots.h"

#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"

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
}

void IUiSlots::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(transform.translate.x, transform.translate.y,
                  transform.scale);
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
  graph_->Render();
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
    data::TextId flip_select_edit_back_text_id,
    data::VboIdMain flip_point_edge_back_vbo_texture,
    data::TextId flip_point_edge_back_text_id,
    UiDynamicSprite&& flip_select_edit_sprite,
    UiDynamicSprite&& flip_point_edge_sprite,
    UiDynamicSprite&& slot_name,
    UiDynamicSprite&& slot_config,
    UiToggle&& toggle_slot_visible,
    UiDynamicSprite&& slot_back,
    UiDynamicSprite&& slot_color,
    UiDynamicSprite&& slot_remove,
    UiDynamicSprite&& slot_selected)
    : IUiSlots(slider.GetId(), ui_shared_resources),
      handler_(std::move(handler)),
      slider_(std::move(slider)),
      back_(std::move(back)),
      create_(std::move(create)),
      flip_select_edit_back_(
          flip_select_edit_back_vbo_texture, flip_select_edit_back_text_id,
          [this]() {
            this->NextSelectMode();
          }),
      flip_point_edge_back_(
          flip_point_edge_back_vbo_texture, flip_point_edge_back_text_id,
          [this]() {
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
      ui_event_handler_({&create_, &flip_select_edit_back_,
                         &flip_point_edge_back_}),
      ui_shared_resources_(ui_shared_resources),
      graph_(ui_shared_resources) {
  gUiComponents[handler_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[slider_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[create_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[flip_select_edit_back_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[flip_point_edge_back_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[flip_select_edit_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[flip_point_edge_sprite_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[slot_name_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[slot_config_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[toggle_slot_visible_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[slot_back_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[slot_color_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[slot_remove_.GetId() - details::kIdOffsetUi].parent_id_
      = slot_back_.GetId();
  gUiComponents[slot_selected_.GetId() - details::kIdOffsetUi].parent_id_
      = slot_back_.GetId();
  gUiComponents[back_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  gUiComponents[slot_back_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);

  sl_data_.length_ = sl_data_.kTrackLengthFactor
                     * (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.length_slots_ = sl_data_.kSlotsLengthFactor
                           * (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.centre_ = (back_.GetTopBorder() + back_.GetBottomBorder()) / 2.0f;

  IUiSlots::ui_event_handler_ = &ui_event_handler_;
  IUiSlots::graph_ = &graph_;
  IUiSlots::ui_edit_ = &ui_edit_;

  IUiSlots::UpdateTransform();
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
      ui_event_handler_({&create_, &flip_select_edit_back_,
                         &flip_point_edge_back_}),
      ui_shared_resources_(other.ui_shared_resources_),
      graph_(other.graph_) {
  gUiComponents[back_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
  gUiComponents[slot_back_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
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
    auto graph_name = graph_.GetNamePtr(i + sl_data_.cur_slots_offset_);
    ui_shared_resources_.global_glfw_callback_data_.text_renderer->
        RenderText(slot_name_, graph_name, 1.0f, next_offset);
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

  ui_edit_.Render();
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

void UiSlotsModels::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  back_.UpdateTransform();
  handler_.UpdateTransform();
  slider_.UpdateTransform();
  create_.UpdateTransform();
  flip_select_edit_back_.UpdateTransform();
  flip_point_edge_back_.UpdateTransform();
  flip_select_edit_sprite_.UpdateTransform();
  flip_point_edge_sprite_.UpdateTransform();
  slot_name_.UpdateTransform();
  slot_back_.UpdateTransform();
  slot_config_.UpdateTransform();
  toggle_slot_visible_.UpdateTransform();
  slot_color_.UpdateTransform();
  slot_remove_.UpdateTransform();
  slot_selected_.UpdateTransform();
  sl_data_.length_ = sl_data_.kTrackLengthFactor * scale *
            (back_.GetTopBorder() - back_.GetBottomBorder());
  sl_data_.length_slots_ = sl_data_.kSlotsLengthFactor * scale *
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
    if (debug::gCtrlMode && (selected_slot_id - sl_data_.cur_slots_offset_) == i) {
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
    auto graph_name = graph_.GetNamePtr(i + sl_data_.cur_slots_offset_);
    ui_shared_resources_.global_glfw_callback_data_.text_renderer->
        RenderTextPicking(slot_name_, graph_name, 1.0f, next_offset);
    next_offset.y -= sl_data_.slot_height_;
  }

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  // we should draw it last (on top of slots)
  if (debug::gCtrlMode && show_selected) {
    slot_selected_.SetTranslate(selected_offset);
    slot_selected_.RenderPicking();
  }
  glDisable(GL_SCISSOR_TEST);

  create_.RenderPicking();
  flip_select_edit_back_.RenderPicking();
  flip_point_edge_back_.RenderPicking();
  if (debug::gCtrlMode) {
    handler_.RenderPicking();
    //    flip_select_edit_.RenderPicking();
    //    flip_point_edge_.RenderPicking();
    flip_select_edit_.RenderPicking();
    flip_point_edge_.RenderPicking();
  }

  ui_edit_.RenderPicking();
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
      sl_data_.UpdateRenderData(back_.GetTopBorder() - back_.GetBottomBorder(),
                                graph_.GetSize());
    }
  } else {
    bool handled = ui_event_handler_.Press(id);
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
