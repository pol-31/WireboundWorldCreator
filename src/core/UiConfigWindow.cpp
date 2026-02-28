#include "UiConfigWindow.h"

#include "../common/Vbos.h"

UiConfigWindow::UiConfigWindow(UiRenderData& render_data,
                               WindowQueue& window_queue,
                               TextRenderer& text_renderer)
    : UiWindowAppear(
          (data::UiId::kConfigWindowDesk), 1.0f,
          {data::UiId::kConfigWindowPinBack, data::UiId::kConfigWindowPinPoint},
          render_data, window_queue),
      name_(text_renderer, data::UiId::kConfigWindowName, data::TextId::kSeed),
      btn_save_(data::UiId::kConfigWindowSave),
      sl_data_({data::UiId::kConfigWindowSlider},
               {data::UiId::kConfigWindowHandler}, 6, 0.75f, 0.8f),
      config_slider_(data::UiId::kConfigWindowSliderArea,
                     data::UiId::kConfigWindowSliderIcon),
      config_text_(text_renderer, data::UiId::kConfigWindowSliderText,
                   data::TextId::kSeed),
      hierarchy_(&background_, {&pin_, &name_, &btn_save_, &sl_data_}) {
  UiBase* comps[] = {&config_text_};
  hierarchy_.AddNested(&config_slider_, comps);
  sl_data_.SetSlotPtr(config_slider_.GetTrackPtr());
  SetupUiHierarchy();
}

void UiConfigWindow::RenderSlotsSprites(glm::vec2 mouse_pos) {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render =
      std::min(sl_data_.slots_num_ - 1, static_cast<int>(value_.size()));
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    config_slider_.SetTranslate(next_offset);
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
  int slots_to_render =
      std::min(sl_data_.slots_num_ - 1, static_cast<int>(text_id_.size()));
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
  auto mouse_pos = render_data_.glfw_context_.cursor_pos_tex_norm_;
  render_data_.shader_sp_.Bind();
  render_data_.tex_ui_.BindSampler(0);
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
  // TODO: config_slider_::handle probably already use SetTranslate
  int slots_to_render =
      std::min(sl_data_.slots_num_ - 1, static_cast<int>(value_.size()));
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    config_slider_.SetTranslate(next_offset);
    config_slider_.SetValue(value_[i]);
    config_slider_.RenderPicking();
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiConfigWindow::RenderPickingSlotsText() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render =
      std::min(sl_data_.slots_num_ - 1, static_cast<int>(text_id_.size()));
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
  render_data_.shader_sp_picking_.Bind();
  render_data_.tex_ui_.BindSampler(0);
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
    sl_pressed_config_ =
        sl_data_.GetSlotId(render_data_.glfw_context_.cursor_pos_tex_norm_);
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

void UiConfigWindow::SetupUiHierarchy() {
  LocalTransform transform;
  float half_width = background_.GetWidth() / 2.0f;
  float quat_width = background_.GetWidth() / 4.0f;
  transform.translate.x = half_width;
  sl_data_.SetParentTransform(transform);
  transform.scale = 0.1f;
  transform.translate.x = -quat_width;
  transform.translate.y = .0f;
  transform.translate.y = background_.GetHeight() / 4.0f;
  //  transform.translate.x = sprite_.GetRightBorder();
  //  transform.translate.y = sprite_.GetTopBorder();
  config_text_.SetParentTransform(transform);
  transform.scale = 0.7f;
  transform.translate.x += half_width;
  config_slider_.SetParentTransform(transform);
}
