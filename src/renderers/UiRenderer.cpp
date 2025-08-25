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
          ui_shared_resources_,
          paths, {data::VboIdMain::kSpareText10},
          {data::VboIdMain::kSpareText9}, {data::VboIdMain::kSpareText8}),
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
          {data::VboIdMain::kSettingsDesk},
          2.0f,
          {{data::VboIdMain::kSettingsDeskPinBack, [](){}},
           {data::VboIdMain::kSettingsDeskPinPoint}},
          ui_shared_resources_,
          windows_,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.2f, 0.0f}, 1.0f, 0.0f},
          {data::VboIdMain::kSettingsResolutionLabel},
          {data::VboIdMain::kSettingsResolutionLeft},
          {data::VboIdMain::kSettingsResolutionRight},
          {data::VboIdMain::kSettingsResolution},
          {{data::VboIdMain::kSettingsFullScreenOff},
           {data::VboIdMain::kSettingsFullScreenOn1},
           {data::VboIdMain::kSettingsFullScreenOn2},
           {data::VboIdMain::kSettingsFullScreenOn3}},
          UiSliderH2{
              {data::VboIdMain::kSettingsSensitivityArea},
              {data::VboIdMain::kSettingsSensitivityHandle},
          },
          UiSliderH2{
              {data::VboIdMain::kSettingsSoundArea},
              {data::VboIdMain::kSettingsSoundHandle},
          },
          {{data::VboIdMain::kSettingsSoundOff},
           {data::VboIdMain::kSettingsSoundOn1},
           {data::VboIdMain::kSettingsSoundOn2},
           {data::VboIdMain::kSettingsSoundOn3}},
          UiSliderH2{
              {data::VboIdMain::kSettingsMusicArea},
              {data::VboIdMain::kSettingsMusicHandle},
          },
          {{data::VboIdMain::kSettingsMusicOff},
           {data::VboIdMain::kSettingsMusicOn1},
           {data::VboIdMain::kSettingsMusicOn2},
           {data::VboIdMain::kSettingsMusicOn3}},
          {data::VboIdMain::kSettingsTipInfoLabel},
          {data::VboIdMain::kSettingsTipInfo},
          {{data::VboIdMain::kSettingsTipInfoOff},
           {data::VboIdMain::kSettingsTipInfoOn1},
           {data::VboIdMain::kSettingsTipInfoOn2},
           {data::VboIdMain::kSettingsTipInfoOn3}}
          ),
      ui_confirmation_(
          {data::VboIdMain::kConfirmationDesk},
          {data::VboIdMain::kSpareText6},
          1.0f,
          ui_shared_resources_,
          windows_,
          {data::VboIdMain::kConfirmationAccept},
          {data::VboIdMain::kConfirmationDecline},
          {data::VboIdMain::kConfirmationTextLabel}
          ),
      ui_caution_(
          {data::VboIdMain::kSpareText7},
          {data::VboIdMain::kSpareText4},
          1.0f,
          ui_shared_resources_,
          windows_,
          {data::VboIdMain::kConfirmationTextLabel}
          ),
      ui_file_(
          {data::VboIdMain::kFileDesk},
          {data::VboIdMain::kSpareText5},
          1.0f,
          ui_shared_resources_,
          windows_,
          {data::VboIdMain::kFileAccept},
          {data::VboIdMain::kRoadsEditAccept},
          {text_renderer_, 1.0f,
           {data::VboIdMain::kFileText}},
          {text_renderer_,
           {data::VboIdMain::kRoadsSlotsSlot},
           {data::VboIdMain::kFileTextInput}}),
      ui_tip_(
          {data::VboIdMain::kTipInfoDesk},
          1.0f,
          {
              {data::VboIdMain::kTipInfoDeskPinBack, [](){}},
              {data::VboIdMain::kTipInfoDeskPinPoint},
          },
          ui_shared_resources_,
          windows_,
          {data::VboIdMain::kTipInfoText}),
      ui_loading_(
          {data::VboIdMain::kLoading0},
          {data::VboIdMain::kLoading1},
          {data::VboIdMain::kLoading2},
          {data::VboIdMain::kLoading3},
          {data::VboIdMain::kLoading4},
          {data::VboIdMain::kLoading5},
          {data::VboIdMain::kLoading6},
          {data::VboIdMain::kLoading7},
          {data::VboIdMain::kLoading8},
          {data::VboIdMain::kLoading9},
          {data::VboIdMain::kLoading10}),
      compass_(data::VboIdMain::kCompass) {
  Init();
}

void UiRenderer::Render() {
#ifndef NDEBUG
  debug::gUiAltMode = glfwGetKey(gWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
  if (debug::gUiAltMode) {
    // it's static, but... idk ui_debugger_.BindCallbacks() looks better
    debug::UiDebugger::BindCallbacks();
    ui_debugger_.ApplyAndReset();
    ui_debugger_.Update();
  } else if (debug_ui_prev_) {
    cur_mode_->BindCallbacks();
    ui_debugger_.ApplyAndReset();
    ui_debugger_.Reset();
  }
  debug_ui_prev_ = debug::gUiAltMode;
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
      cur_mode_->BindCallbacks();
    }
  }
  if (text_renderer_.InputInProgress()) {
    text_renderer_.RenderInput();
  }
}

void UiRenderer::RenderPicking() {
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  compass_.RenderPicking();

  cur_mode_->RenderPicking();
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
  text_renderer_.PrerenderMenuText(0, 58);
  ui_debugger_.ForceUpdate();
}

void UiRenderer::SetupGlobalData() {
  ui_shared_resources_.global_glfw_callback_data_.windows = &windows_;
  ui_shared_resources_.global_glfw_callback_data_.cur_mode = &cur_mode_;
  ui_shared_resources_.global_glfw_callback_data_.menu = &menu_;
  ui_shared_resources_.global_glfw_callback_data_.ui_debugger = &ui_debugger_;
  ui_shared_resources_.global_glfw_callback_data_.ui_shared_resources = &ui_shared_resources_;
  ui_shared_resources_.global_glfw_callback_data_.text_renderer = &text_renderer_;
  ui_shared_resources_.global_glfw_callback_data_.ui_renderer = this;
}

void UiRenderer::AskForConfirmation(
    std::string_view text, std::function<void()>&& callable) {
  ui_confirmation_.SetText(text);
  ui_confirmation_.SetCallable(std::move(callable));
  ui_confirmation_.Show();
}
