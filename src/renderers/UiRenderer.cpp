#include "UiRenderer.h"

#include "../io/Cameras.h"

UiRenderer::UiRenderer(
    const Paths& paths, GlobalGlfwCallbackData& global_glfw_data_,
    TileRenderer& tile_renderer)
    : ui_shared_resources_(paths, global_glfw_data_),
      ui_debugger_(paths, ui_shared_resources_.vbo_ui_,
                   ui_shared_resources_.vbo_ui_transform_,
                   global_glfw_data_.cursor_pos_tex_norm_),
      windows_(),
      text_renderer_(
          paths, {data::VboIdMain::kSpareText10, data::TextId::kNotYet}),
      terrain_(ui_shared_resources_, windows_, text_renderer_, tile_renderer.cur_tile_),
      water_(ui_shared_resources_, windows_, paths),
      fences_(ui_shared_resources_, text_renderer_, windows_),
      roads_(ui_shared_resources_, windows_),
      biomes_(ui_shared_resources_, windows_),
      objects_(ui_shared_resources_, windows_),
      placement_(ui_shared_resources_, windows_, paths),
      tiles_(ui_shared_resources_, windows_),
      menu_(ui_shared_resources_, windows_, text_renderer_, &terrain_,
            &water_, &roads_, &fences_, &placement_, &objects_, &biomes_,
            &tiles_, cur_mode_),
      ui_settings_(
          {data::VboIdMain::kSettingsDesk, data::TextId::kNotYet},
          4.0f,
          {{data::VboIdMain::kSettingsDeskPinBack, data::TextId::kNotYet, [](){}},
           {data::VboIdMain::kSettingsDeskPinPoint, data::TextId::kNotYet}},
          ui_shared_resources_,
          windows_,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.2f, 0.0f}, 1.0f, 0.0f},
          {data::VboIdMain::kSettingsResolutionLabel, data::TextId::kNotYet},
          {data::VboIdMain::kSettingsResolutionLeft, data::TextId::kNotYet},
          {data::VboIdMain::kSettingsResolutionRight, data::TextId::kNotYet},
          {data::VboIdMain::kSettingsResolutionSlot, data::TextId::kNotYet},
          {data::VboIdMain::kSettingsResolution, data::TextId::kNotYet},
          {{data::VboIdMain::kSettingsFullScreenOff, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsFullScreenOn1, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsFullScreenOn2, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsFullScreenOn3, data::TextId::kNotYet}},
          {data::VboIdMain::kSettingsSensitivityIcon, data::TextId::kNotYet},
          UiSliderH2{
              {data::VboIdMain::kSettingsSensitivityArea, data::TextId::kNotYet},
              {data::VboIdMain::kSettingsSensitivityHandle, data::TextId::kNotYet},
          },
          {data::VboIdMain::kSettingsKeyboard, data::TextId::kNotYet},
          {data::VboIdMain::kSettingsSoundIcon, data::TextId::kNotYet},
          UiSliderH2{
              {data::VboIdMain::kSettingsSoundArea, data::TextId::kNotYet},
              {data::VboIdMain::kSettingsSoundHandle, data::TextId::kNotYet},
          },
          {{data::VboIdMain::kSettingsSoundOff, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsSoundOn1, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsSoundOn2, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsSoundOn3, data::TextId::kNotYet}},
          {data::VboIdMain::kSettingsMusicIcon, data::TextId::kNotYet},
          UiSliderH2{
              {data::VboIdMain::kSettingsMusicArea, data::TextId::kNotYet},
              {data::VboIdMain::kSettingsMusicHandle, data::TextId::kNotYet},
          },
          {{data::VboIdMain::kSettingsMusicOff, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsMusicOn1, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsMusicOn2, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsMusicOn3, data::TextId::kNotYet}},
          {data::VboIdMain::kSettingsTipInfoLabel, data::TextId::kNotYet},
          {data::VboIdMain::kSettingsTipInfo, data::TextId::kNotYet},
          {{data::VboIdMain::kSettingsTipInfoOff, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsTipInfoOn1, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsTipInfoOn2, data::TextId::kNotYet},
           {data::VboIdMain::kSettingsTipInfoOn3, data::TextId::kNotYet}}
          ),
      ui_confirmation_(
          {data::VboIdMain::kConfirmationDesk, data::TextId::kNotYet},
          {data::VboIdMain::kSpareText6, data::TextId::kNotYet},
          1.0f,
          ui_shared_resources_,
          windows_,
          {data::VboIdMain::kConfirmationAccept, data::TextId::kNotYet},
          {data::VboIdMain::kConfirmationDecline, data::TextId::kNotYet},
          {data::VboIdMain::kConfirmationTextLabel, data::TextId::kNotYet}
          ),
      ui_caution_(
          {data::VboIdMain::kSpareText7, data::TextId::kNotYet},
          {data::VboIdMain::kSpareText4, data::TextId::kNotYet},
          1.0f,
          ui_shared_resources_,
          windows_,
          {data::VboIdMain::kConfirmationTextLabel, data::TextId::kNotYet}
          ),
      ui_file_(
          {data::VboIdMain::kFileDesk, data::TextId::kNotYet},
          {data::VboIdMain::kSpareText5, data::TextId::kNotYet},
          1.0f,
          ui_shared_resources_,
          windows_,
          {data::VboIdMain::kFileAccept, data::TextId::kNotYet},
          {data::VboIdMain::kRoadsEditAccept, data::TextId::kNotYet},
          {text_renderer_, 1.0f, glm::vec2{0.0f},
           {data::VboIdMain::kFileText, data::TextId::kNotYet}},
          {text_renderer_, 1.0f, glm::vec2{0.0f},
           {data::VboIdMain::kRoadsSlotsSlot, data::TextId::kNotYet},
           {data::VboIdMain::kFileTextInput, data::TextId::kNotYet}}),
      ui_tip_(
          {data::VboIdMain::kTipInfoDesk, data::TextId::kNotYet},
          1.0f,
          {
              {data::VboIdMain::kTipInfoDeskPinBack, data::TextId::kNotYet, [](){}},
              {data::VboIdMain::kTipInfoDeskPinPoint, data::TextId::kNotYet},
          },
          ui_shared_resources_,
          windows_,
          {data::VboIdMain::kTipInfoText, data::TextId::kNotYet}),
      ui_loading_(
          {data::VboIdMain::kLoading0, data::TextId::kNotYet},
          {data::VboIdMain::kLoading1, data::TextId::kNotYet},
          {data::VboIdMain::kLoading2, data::TextId::kNotYet},
          {data::VboIdMain::kLoading3, data::TextId::kNotYet},
          {data::VboIdMain::kLoading4, data::TextId::kNotYet},
          {data::VboIdMain::kLoading5, data::TextId::kNotYet},
          {data::VboIdMain::kLoading6, data::TextId::kNotYet},
          {data::VboIdMain::kLoading7, data::TextId::kNotYet},
          {data::VboIdMain::kLoading8, data::TextId::kNotYet},
          {data::VboIdMain::kLoading9, data::TextId::kNotYet},
          {data::VboIdMain::kLoading10, data::TextId::kNotYet}),
      compass_({data::VboIdMain::kCompass, data::TextId::kNotYet}) {
  Init();
}

void UiRenderer::Render(data::TextId description_id) {
#ifndef NDEBUG
  debug::gCtrlMode = glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
  if (debug::gCtrlMode) {
    // it's static, but... idk ui_debugger_.BindCallbacks() looks better
    debug::UiDebugger::BindCallbacks();
    ui_debugger_.ApplyAndReset();
    ui_debugger_.Update();
  } else if (debug_ui_prev_) {
    cur_mode_->BindCallbacks();
    ui_debugger_.ApplyAndReset();
    ui_debugger_.Reset();
  }
  debug_ui_prev_ = debug::gCtrlMode;
#endif // NDEBUG

  cur_mode_->Render();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  compass_.SetRotate(glm::radians(
      ui_shared_resources_.global_glfw_callback_data_.camera->GetYaw()));
  compass_.Render();

  bool old_render_menu = render_menu_;
  render_menu_ = (glfwGetKey(gWindow, GLFW_KEY_TAB) == GLFW_PRESS);
//  menu_.Render(render_menu_);
  if (old_render_menu != render_menu_) {
    if (render_menu_) {
      menu_.BindCallbacks();
      menu_.Show();
      ui_settings_.Show();
    } else {
      menu_.Hide();
      ui_settings_.Hide();
//      ui_file_.Show();
//      ui_file_.Hide();
      cur_mode_->BindCallbacks();
    }
  }

  if (render_menu_) {
//    text_renderer_.RenderMenuText(description_id);
  } else {
//    text_renderer_.RenderModeText(description_id);
  }
}

data::TextId UiRenderer::Hover(GLuint pressed_id) {
  data::TextId description_id;
  description_id = cur_mode_->Hover(pressed_id);
  if (description_id != data::TextId::kNone) {
  }
  /*description_id = menu_.Hover(render_menu_, pressed_id);
  if (render_menu_) {
    if (description_id == data::TextId::kNone) {
      description_id = data::TextId::kMenu;
    }
  } else {
    description_id = cur_mode_->Hover(pressed_id);
    if (description_id != data::TextId::kNone) {
    }
  }*/
   menu_.Hover(render_menu_, pressed_id);
  return description_id;
}

void UiRenderer::RenderPicking(data::TextId description_id) {
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  compass_.RenderPicking();

  cur_mode_->RenderPicking();
  if (description_id != data::TextId::kNone) {
    //      text_renderer_.RenderModeTextPicking(description_id);
  }
  return;

  menu_.RenderPicking(render_menu_);
  if (description_id != data::TextId::kNone) {
    //      text_renderer_.RenderMenuTextPicking(description_id);
  }
  if (!render_menu_) {
    cur_mode_->RenderPicking();
    if (description_id != data::TextId::kNone) {
//      text_renderer_.RenderModeTextPicking(description_id);
    }
  }
}

void UiRenderer::Parse() {
  menu_.Parse();
  terrain_.Parse();
  water_.Parse();
  fences_.Parse();
  roads_.Parse();
  biomes_.Parse();
  objects_.Parse();
  placement_.Parse();
  tiles_.Parse();
}

void UiRenderer::Serialize() {
  menu_.Serialize();
  terrain_.Serialize();
  water_.Serialize();
  fences_.Serialize();
  roads_.Serialize();
  biomes_.Serialize();
  objects_.Serialize();
  placement_.Serialize();
  tiles_.Serialize();
}

void UiRenderer::Init() {
  text_renderer_.PrerenderMenuText(0, 24);
  ui_debugger_.ForceUpdate();
}

void UiRenderer::SetupGlobalData() {
  ui_shared_resources_.global_glfw_callback_data_.windows = &windows_;
  ui_shared_resources_.global_glfw_callback_data_.cur_mode = &cur_mode_;
  ui_shared_resources_.global_glfw_callback_data_.menu = &menu_;
  ui_shared_resources_.global_glfw_callback_data_.ui_debugger = &ui_debugger_;
  ui_shared_resources_.global_glfw_callback_data_.ui_shared_resources = &ui_shared_resources_;
  ui_shared_resources_.global_glfw_callback_data_.text_renderer = &text_renderer_;
}
