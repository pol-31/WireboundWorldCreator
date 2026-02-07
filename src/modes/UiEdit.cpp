#include "UiEdit.h"

#include <glm/gtc/type_ptr.hpp>

UiEditSlots::UiEditSlots(UiRenderData& render_data,
                         WindowQueue& window_queue, TextRenderer& text_renderer)
    : Base({data::VboIdMain::kBiomesEditDesk, [] {}}, 1.0f,
           {{data::VboIdMain::kBiomesEditDeskPinBack, [] {}},
            {data::VboIdMain::kBiomesEditDeskPinPoint}},
           render_data, window_queue),
      accept_(data::VboIdMain::kBiomesEditAccept,
              [this] { ui_edit_->Generate(); }),
      name_(text_renderer, {data::VboIdMain::kBiomesEditNameBack},
            {data::VboIdMain::kBiomesEditName}),
      color_palette_({data::VboIdMain::kBiomesColorPaletteHS},
                     {data::VboIdMain::kBiomesColorColorCursor},
                     glm::vec2{1.0f}),
      color_brightness_({data::VboIdMain::kBiomesColorPaletteB},
                        {data::VboIdMain::kBiomesColorBrightnessCursor}, 1.0f),
      color_indicator_(data::VboIdMain::kBiomesColorIndicator),
      random_generate_(data::VboIdMain::kBiomesEditRandomGeneration,
                       [this] { ui_edit_->RandomGenerate(); }),
      ui_event_handler_({&pin_, &accept_, &name_, &color_palette_,
                         &color_brightness_, &random_generate_}),
      hierarchy_(&background_,
                 {&pin_, &accept_, &name_, &color_palette_, &color_brightness_,
                  &color_indicator_, &random_generate_}),
      render_data_(render_data),
      random_generator_(std::random_device{}()) {
  speed_ = 2.0f;
  color_brightness_.SetValue(1.0f);
}

void UiEditSlots::SetUp(IUiEdit* ui_edit) { ui_edit_ = ui_edit; }

void UiEditSlots::SetInstance(int id) {
  auto data = ui_edit_->GetBaseInstanceData(id);
  glm::vec3 hsb = RGBtoHSB(data->color);
  color_palette_.SetValue({hsb.x, hsb.y});
  color_brightness_.SetValue(hsb.z);
  name_.SetText(data->name);
}

bool UiEditSlots::Press(int id) {
  if (ui_edit_->Press(id, background_.GetHeight())) {
    return true;
  }
  return ui_event_handler_.Press(id);
}

void UiEditSlots::Release() {
  ui_edit_->Release();
  return ui_event_handler_.Release();
}

bool UiEditSlots::Scroll(GLuint id, float yoffset) {
  return color_palette_.Scroll(id, yoffset) ||
         color_brightness_.Scroll(id, yoffset);
}

bool UiEditSlots::Render() {
  render_data_.tex_ui_.BindSampler(0);
  auto mouse_pos = render_data_.glfw_context_.cursor_pos_tex_norm_;
  bool stop_show = RenderBack(true);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  render_data_.shader_sp_.Bind();
  render_data_.tex_ui_.BindSampler(0);
  accept_.Render();
  color_palette_.Render(mouse_pos);
  color_brightness_.Render(mouse_pos);

  auto model_data =
      ui_edit_->GetBaseInstanceData(*ui_edit_->GetSelectedIdPtr());

  model_data->color =
      HSBtoRGB(color_palette_.GetProgressX() * 0.85f,
               color_palette_.GetProgressY(), color_brightness_.GetProgress());
  glUniform4fv(7, 1, glm::value_ptr(model_data->color));
  color_indicator_.Render();
  auto color = glm::vec4{1.0f};
  glUniform4fv(7, 1, glm::value_ptr(color));

  random_generate_.Render();
  name_.RenderBack();
  ui_edit_->Render(background_.GetHeight());

  model_data->name = name_.GetText();
  name_.RenderText();
  return false;
}

void UiEditSlots::RenderPicking() {
  RenderPickingBack();
  render_data_.shader_sp_picking_.Bind();

  accept_.RenderPicking();
  name_.RenderPicking();
  color_palette_.RenderPicking();
  color_brightness_.RenderPicking();
  color_indicator_.RenderPicking();
  random_generate_.RenderPicking();

  ui_edit_->RenderPicking(background_.GetHeight());

  render_data_.shader_sp_picking_.Bind();
  name_.RenderPicking();
}
