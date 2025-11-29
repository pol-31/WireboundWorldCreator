#include "UiConfigWindow.h"

#include "../common/Vbos.h"

UiConfigWindow::UiConfigWindow(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : UiWindowAppear(
          (data::VboIdMain::kConfigWindowDesk), 1.0f,
          {data::VboIdMain::kConfigWindowPinBack,
           data::VboIdMain::kConfigWindowPinPoint},
          ui_shared_resources, window_queue),
      name_(text_renderer, data::VboIdMain::kConfigWindowName,
            data::TextId::kSeed),
      btn_save_(data::VboIdMain::kConfigWindowSave),
      sl_data_({data::VboIdMain::kConfigWindowSlider},
               {data::VboIdMain::kConfigWindowHandler},
               6, 0.75f, 0.8f),
      config_slider_(data::VboIdMain::kConfigWindowSliderArea,
                     data::VboIdMain::kConfigWindowSliderIcon),
      config_text_(text_renderer, data::VboIdMain::kConfigWindowSliderText,
                   data::TextId::kSeed) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_, &sl_data_);
  hierarchy_.AddNested(&config_slider_, &config_text_);
  sl_data_.SetSlotPtr(config_slider_.GetTrackPtr());
}

UiConfigWindow::UiConfigWindow(UiConfigWindow&& other) noexcept
    : UiWindowAppear(std::move(other)),
      name_(std::move(other.name_)),
      btn_save_(std::move(other.btn_save_)),
      sl_data_(std::move(other.sl_data_)),
      config_slider_(std::move(other.config_slider_)),
      config_text_(std::move(other.config_text_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_, &sl_data_);
  hierarchy_.AddNested(&config_slider_, &config_text_);
}

void UiConfigWindow::RenderSlotsSprites(glm::vec2 mouse_pos) {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_ - 1, static_cast<int>(value_.size()));
  LocalTransform transform;
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    transform.translate = next_offset;
    //    config_slider_.SetTranslate(next_offset);
    config_slider_.SetParentTransform(transform);
    if (i == sl_pressed_config_) {
      config_slider_.Render(mouse_pos);
      value_[i] = config_slider_.GetProgress();
    } else {
      config_slider_.SetValue(value_[i]);
      config_slider_.Render();
    }
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiConfigWindow::RenderSlotsText() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_ - 1, static_cast<int>(text_id_.size()));
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    auto graph_name = text_id_[i];
    config_text_.SetText(graph_name);
    config_text_.SetTranslate(next_offset);
    config_text_.Render();
    next_offset.y -= sl_data_.slot_height_;
  }
}

bool UiConfigWindow::Render() {
  RenderBack(true);
  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  btn_save_.Render();
  sl_data_.Render(mouse_pos);
  glEnable(GL_SCISSOR_TEST);
  sl_data_.SetUpScissors();
  RenderSlotsSprites(mouse_pos);
  RenderSlotsText();
  glDisable(GL_SCISSOR_TEST);
  name_.Render();
  return false;
}

void UiConfigWindow::RenderPickingSlotsSprites() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  //TODO: config_slider_::handle probably already use SetTranslate
  int slots_to_render = std::min(
      sl_data_.slots_num_ - 1, static_cast<int>(value_.size()));
  LocalTransform transform;
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    transform.translate = next_offset;
    config_slider_.SetParentTransform(transform);
    config_slider_.SetValue(value_[i]);
    config_slider_.RenderPicking();
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiConfigWindow::RenderPickingSlotsText() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_ - 1, static_cast<int>(text_id_.size()));
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    auto graph_name = text_id_[i];
    config_text_.SetText(graph_name);
    config_text_.SetTranslate(next_offset);
    config_text_.RenderPicking();
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiConfigWindow::RenderPicking() {
  RenderPickingBack();
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  btn_save_.RenderPicking();
  sl_data_.RenderPicking();
  glEnable(GL_SCISSOR_TEST);
  sl_data_.SetUpScissors();
  RenderPickingSlotsSprites();
  RenderPickingSlotsText();
  glDisable(GL_SCISSOR_TEST);
  name_.RenderPicking();
}

bool UiConfigWindow::Scroll(GLuint id, float yoffset) {
  // any inside
  return false;
  //  return slider_.Scroll(id, yoffset);
}

bool UiConfigWindow::Press(int id) {
  if (id == pin_.GetId()) {
    pin_.Press();
  } else if (id == sl_data_.GetId()) {
    sl_data_.Press();
    return true;
  } else if (id == config_slider_.GetId()) {
    sl_pressed_config_ = sl_data_.GetSlotId(
        ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
    std::cout << sl_pressed_config_ << std::endl;
    config_slider_.Press();
    return true;
  }
  return false;
}

void UiConfigWindow::Release() {
  sl_data_.Release();
  sl_pressed_config_ = -1;
  config_slider_.Release();
}

void UiConfigWindow::SetNoise(std::span<float> value,
                              std::span<data::TextId> text_id,
                              data::TextId name_id) {
  value_ = value;
  text_id_ = text_id;
  name_.SetText(name_id);
  sl_data_.SetEntryNum(static_cast<int>(value_.size()));
}
