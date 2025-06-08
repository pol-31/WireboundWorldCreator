#include "UiSlots.h"

#include "../common/TextRenderer.h"
#include "../common/UiDebugger.h"

const float UiSlots::kTrackLengthFactor = 0.75f;
const float UiSlots::kSlotsLengthFactor = 0.8f;
const int UiSlots::kSlotsNum = 6;

/// parent is back_ BUT UiSlots is taken from slider, so
/// outside is's shown as a slider area
UiSlots::UiSlots(
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
    UiDynamicSprite&& slot_back,
    UiDynamicSprite&& slot_remove,
    UiDynamicSprite&& slot_selected,
    ArbitraryGraph& graph)
    : UiCallable(slider.GetId(), {}),
      handler_(std::move(handler)),
      slider_(std::move(slider)),
      back_(std::move(back)),
      create_(std::move(create)),
      flip_select_edit_back_(
          flip_select_edit_back_vbo_texture, flip_select_edit_back_text_id,
          [this]() {
            this->FlipSelectEdit();
          }),
      flip_point_edge_back_(
          flip_point_edge_back_vbo_texture, flip_point_edge_back_text_id,
          [this]() {
            this->FlipPointEdge();
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
      slot_back_(std::move(slot_back)),
      slot_remove_(std::move(slot_remove)),
      slot_selected_(std::move(slot_selected)),
      length_(kTrackLengthFactor * (back_.GetTopBorder()
                                    - back_.GetBottomBorder())),
      length_slots_(kSlotsLengthFactor * (back_.GetTopBorder()
                                          - back_.GetBottomBorder())),
      centre_((back_.GetTopBorder()
               + back_.GetBottomBorder()) / 2.0f),
      ui_event_handler_({&create_, &flip_select_edit_back_,
                         &flip_point_edge_back_}),
      ui_shared_resources_(ui_shared_resources),
      graph_(graph) {
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
  gUiComponents[slot_back_.GetId() - details::kIdOffsetUi].parent_id_
      = back_.GetId();
  gUiComponents[slot_remove_.GetId() - details::kIdOffsetUi].parent_id_
      = slot_back_.GetId();
  gUiComponents[slot_selected_.GetId() - details::kIdOffsetUi].parent_id_
      = slot_back_.GetId();
  gUiComponents[back_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiTransformDbg*>(this);
  gUiComponents[slot_back_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiTransformDbg*>(this);
  UpdateTransform();
}

UiSlots::UiSlots(UiSlots&& other) noexcept
    : UiCallable(std::move(other)),
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
      slot_back_(std::move(other.slot_back_)),
      slot_remove_(std::move(other.slot_remove_)),
      slot_selected_(std::move(other.slot_selected_)),
      cur_slots_offset_(other.cur_slots_offset_),
      start_slot_translate_(other.start_slot_translate_),
      ui_event_handler_({&create_, &flip_select_edit_back_,
                         &flip_point_edge_back_}),
      ui_shared_resources_(other.ui_shared_resources_),
      graph_(other.graph_) {
  progress_ = other.progress_;
  pressed_ = other.pressed_;
  centre_ = other.centre_;
  length_ = other.length_;
  length_slots_ = other.length_slots_;
  gUiComponents[back_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
  gUiComponents[slot_back_.GetId() - details::kIdOffsetUi].ui =
      static_cast<UiTransformDbg*>(this);
}

void UiSlots::UpdateRenderData() {
  auto graphs_num = graph_.GetGraphNum();
  // total 6, visible 5
  slot_height_ = kSlotsLengthFactor * (slot_back_.GetTopBorder() - slot_back_.GetBottomBorder());
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

void UiSlots::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  back_.Render();
  slider_.Render();
  handler_.Render();
  glEnable(GL_SCISSOR_TEST);
  glScissor(0, scissors_start_, 4000, scissors_length_);

  bool show_selected = false;
  glm::vec2 selected_offset{0.0f};
  auto selected_slot_id = graph_.GetSlotId();
  auto graphs_num = graph_.GetGraphNum();
  glm::vec2 next_offset = start_slot_translate_;
  for (int i = 0; i < std::min(kSlotsNum, graphs_num); ++i) {
    slot_back_.SetTranslate(next_offset);
    slot_remove_.SetTranslate(next_offset);
    if (selected_slot_id - cur_slots_offset_ == i) {
      show_selected = true;
      selected_offset = next_offset;
    }
    slot_back_.Render();
    slot_remove_.Render();
    next_offset.y -= slot_height_;
  }
  next_offset = start_slot_translate_;
  for (int i = 0; i < std::min(kSlotsNum, graphs_num - cur_slots_offset_); ++i) {
    auto graph_name = graph_.GetNamePtr(i + cur_slots_offset_);
    ui_shared_resources_.global_glfw_callback_data_.text_renderer->
        RenderText(graph_name, 1.0f, next_offset);
    next_offset.y -= slot_height_;
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
}

void UiSlots::RenderPicking() {
  back_.RenderPicking();
  slider_.RenderPicking();
  glEnable(GL_SCISSOR_TEST);
  glScissor(0, scissors_start_, 4000, scissors_length_);

  bool show_selected = false;
  glm::vec2 selected_offset{0.0f};
  auto selected_slot_id = graph_.GetSlotId();
  auto graphs_num = graph_.GetGraphNum();
  glm::vec2 next_offset = start_slot_translate_;
  for (int i = 0; i < std::min(kSlotsNum, graphs_num); ++i) {
    slot_back_.SetTranslate(next_offset);
    slot_remove_.SetTranslate(next_offset);
    if (debug::gCtrlMode && (selected_slot_id - cur_slots_offset_) == i) {
      show_selected = true;
      selected_offset = next_offset;
    }
    slot_back_.RenderPicking();
    slot_remove_.RenderPicking();
    next_offset.y -= slot_height_;
  }

  next_offset = start_slot_translate_;
  for (int i = 0; i < std::min(kSlotsNum, graphs_num - cur_slots_offset_); ++i) {
    auto graph_name = graph_.GetNamePtr(i + cur_slots_offset_);
    ui_shared_resources_.global_glfw_callback_data_.text_renderer->
        RenderTextPicking(graph_name, 1.0f, next_offset);
    next_offset.y -= slot_height_;
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
}

int UiSlots::GetSlotId() {
  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_.y;
  float half_slot_height = slot_height_ / 2.0f;
  float border = centre_ + length_slots_ / 2.0f + half_slot_height + start_slot_translate_.y;
  for (int i = 0; i < 5; ++i) {
    if (mouse_pos > border) {
      return i + cur_slots_offset_;
    }
    border -= slot_height_;
  }
  return 5 + cur_slots_offset_; // else cond
}

void UiSlots::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_slots_ / 2.0f;
  float offset = glm::clamp(
      mouse_pos.y - centre_, -half_length_, +half_length_);
  progress_ = 1.0 - (offset + half_length_) / length_slots_;
  glm::vec2 translate = {0.0f, offset};
  handler_.SetTranslate(translate);
  UpdateRenderData();
}

void UiSlots::Set(float progress) {
  progress_ = progress;
  float half_length_ = length_slots_ / 2.0f;
  float offset = -((progress_ - 1.0f) * length_slots_) - half_length_;
  glm::vec2 translate = {0.0f, offset};
  handler_.SetTranslate(translate);
  UpdateRenderData();
}

size_t UiSlots::Hover(std::uint32_t id) {
  return slider_.Hover();
}

void UiSlots::Press() {
  pressed_ = true;
}

void UiSlots::FocusOnSelected(int slot_id) {
  if (slot_id == -1 || graph_.GetGraphNum() < kSlotsNum) {
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
  auto graphs_num = graph_.GetGraphNum();
  auto scrollable_slots = static_cast<float>(std::max(graphs_num - 5, 0));
  std::cout << "was " << progress_;
  progress_ = float_index / scrollable_slots;
  std::cout << " become " << progress_ << std::endl;
  Set(progress_);
}

bool UiSlots::Press(int id) {
  if (id == slot_back_.GetId()) {
    auto slot_id = GetSlotId();
    graph_.SelectGraph(slot_id);
    FocusOnSelected(slot_id);
  } else if (id == slot_remove_.GetId()) {
    auto removed_id = GetSlotId();
    if (graph_.GetGraphNum() > 0) {
      std::cout << "graph removed " << removed_id << std::endl;
      graph_.RemoveGraph(removed_id);
      UpdateRenderData();
    }
  } else {
    bool handled = ui_event_handler_.Press(id);
    if (id == create_.GetId()) {
      // instead of FocusOnSelected();
      Set(1.0f);
    }
    return handled;
  }
  return true;
}

void UiSlots::Release() {
  pressed_ = false;
  ui_event_handler_.Release();
}

bool UiSlots::Scroll(GLuint id, float yoffset) {
  if (id < slot_back_.GetId() ||
      id > flip_select_edit_sprite_.GetId()) {
    return false;
  }
  float normalized_yoffset = 0.2f * yoffset;
  float factor = 1.0f / std::max(graph_.GetGraphNum() - kSlotsNum, 1);
  float progress = std::clamp(progress_ - normalized_yoffset * factor, 0.0f, 1.0f);
  Set(progress);
  return true;
}

void UiSlots::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  back_.UpdateTransform();
  handler_.UpdateTransform();
  slider_.UpdateTransform();
  create_.UpdateTransform();
  flip_select_edit_back_.UpdateTransform();
  flip_point_edge_back_.UpdateTransform();
  flip_select_edit_sprite_.UpdateTransform();
  flip_point_edge_sprite_.UpdateTransform();
  slot_back_.UpdateTransform();
  slot_remove_.UpdateTransform();
  slot_selected_.UpdateTransform();
  length_ = kTrackLengthFactor * scale *
            (back_.GetTopBorder() - back_.GetBottomBorder());
  length_slots_ = kSlotsLengthFactor * scale *
                  (back_.GetTopBorder() - back_.GetBottomBorder());
  centre_ = (back_.GetTopBorder() + back_.GetBottomBorder()) / 2.0f;
  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({0.0f, related_pos});
}

void UiSlots::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(transform.translate.x, transform.translate.y,
                  transform.scale);
}

void UiSlots::FlipSelectEdit() {
  bool edit_mode = graph_.FlipPressMode();
  flip_select_edit_.StopAnimation();
  if (edit_mode) {
    flip_select_edit_.SetStart(
        {glm::vec2{0.0f}, 1.0f, 0.0f});
    flip_select_edit_.SetEnd(
        {glm::vec2{0.0f}, 1.0f, glm::pi<float>()});
  } else {
    flip_select_edit_.SetStart(
        {glm::vec2{0.0f}, 1.0f, glm::pi<float>()});
    flip_select_edit_.SetEnd(
        {glm::vec2{0.0f}, 1.0f, glm::two_pi<float>()});
  }
  flip_select_edit_.RunAnimation();
}

void UiSlots::FlipPointEdge() {
  bool point_mode = graph_.FlipPointsMode();
  flip_point_edge_.StopAnimation();
  if (point_mode) {
    flip_point_edge_.SetStart(
        {glm::vec2{0.0f}, 1.0f, 0.0f});
    flip_point_edge_.SetEnd(
        {glm::vec2{0.0f}, 1.0f, glm::pi<float>()});
  } else {
    flip_point_edge_.SetStart(
        {glm::vec2{0.0f}, 1.0f, glm::pi<float>()});
    flip_point_edge_.SetEnd(
        {glm::vec2{0.0f}, 1.0f, glm::two_pi<float>()});
  }
  flip_point_edge_.RunAnimation();
}
