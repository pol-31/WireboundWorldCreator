#include "UiRenderer.h"

#include "../io/Camera.h"

UiRenderer::UiRenderer(GlfwContext& global_glfw_data_,
                       TileRenderer& tile_renderer, const Camera* camera)
    : ui_shared_resources_(global_glfw_data_),
      mdl_manager_(ui_shared_resources_),
      ui_debugger_(ui_shared_resources_.vbo_ui_,
                   ui_shared_resources_.vbo_ui_transform_,
                   global_glfw_data_.cursor_pos_tex_norm_),
      windows_(),
      text_renderer_(ui_shared_resources_,
                     {data::VboIdMain::kTextRendererPrerenderSlot},
                     {data::VboIdMain::kTextRendererCursor}),
      ui_slots_(ui_shared_resources_, text_renderer_),
      ui_config_window_(ui_shared_resources_, windows_, text_renderer_),
      terrain_(ui_shared_resources_, ui_slots_, windows_, text_renderer_,
               tile_renderer.cur_tile_, ui_config_window_),
      water_(ui_shared_resources_, ui_slots_, windows_, text_renderer_,
             ui_config_window_, mdl_manager_),
      biomes_(ui_shared_resources_, ui_slots_, windows_, text_renderer_),
      objects_(ui_shared_resources_, ui_slots_, windows_, text_renderer_,
               mdl_manager_),
      placement_(ui_shared_resources_, windows_, mdl_manager_),
      tiles_(ui_shared_resources_, windows_),
      player_(ui_shared_resources_, windows_, text_renderer_,
              tile_renderer.cur_tile_, mdl_manager_),
      ui_menu_(ui_shared_resources_, windows_, &terrain_, &water_, &placement_,
               &objects_, &biomes_, &tiles_, &player_, cur_mode_),
      ui_settings_({data::VboIdMain::kSettingsDesk}, 2.0f,
                   {{data::VboIdMain::kSettingsDeskPinBack, []() {}},
                    {data::VboIdMain::kSettingsDeskPinPoint}},
                   ui_shared_resources_, windows_,
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
                    {data::VboIdMain::kSettingsTipInfoOn3}}),
      ui_confirmation_({data::VboIdMain::kConfirmationDesk}, 1.0f,
                       ui_shared_resources_, windows_,
                       {data::VboIdMain::kConfirmationAccept},
                       {data::VboIdMain::kConfirmationDecline},
                       {text_renderer_, data::VboIdMain::kConfirmationText}),
      ui_caution_({data::VboIdMain::kCautionDesk}, 1.0f, ui_shared_resources_,
                  windows_, {data::VboIdMain::kCautionText}),
      ui_file_({data::VboIdMain::kFileDesk}, 1.0f, ui_shared_resources_,
               windows_, {data::VboIdMain::kFileAccept},
               {data::VboIdMain::kFileDecline},
               {text_renderer_, {data::VboIdMain::kFileQuestion}},
               {text_renderer_,
                {data::VboIdMain::kFileTextLabel},
                {data::VboIdMain::kFileTextBack}}),
      ui_tip_({data::VboIdMain::kTipInfoDesk}, 1.0f,
              {
                  {data::VboIdMain::kTipInfoDeskPinBack, []() {}},
                  {data::VboIdMain::kTipInfoDeskPinPoint},
              },
              ui_shared_resources_, windows_, {data::VboIdMain::kTipInfoText}),
      ui_loading_({data::VboIdMain::kLoading0}, {data::VboIdMain::kLoading1},
                  {data::VboIdMain::kLoading2}, {data::VboIdMain::kLoading3},
                  {data::VboIdMain::kLoading4}, {data::VboIdMain::kLoading5},
                  {data::VboIdMain::kLoading6}, {data::VboIdMain::kLoading7},
                  {data::VboIdMain::kLoading8}, {data::VboIdMain::kLoading9},
                  {data::VboIdMain::kLoading10}),
      ui_compass_(
          camera, {data::VboIdMain::kCompass}, {data::VboIdMain::kCompassNorth},
          {data::VboIdMain::kCompassSouth}, {data::VboIdMain::kCompassEast},
          {data::VboIdMain::kCompassWest}),
      ui_layer_wireframe_(ui_shared_resources_),
      ui_world_origin_(ui_shared_resources_),
      ui_buttons_(ui_shared_resources_, text_renderer_),
      ui_grid_(ui_shared_resources_) {
  Init();
}

void UiRenderer::Render() {
  cur_mode_->RenderWorld();
  cur_mode_->Render();
#ifndef NDEBUG
  debug::gUiAltMode = glfwGetKey(gWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
  if (debug::gUiAltMode) {
    // it's static, but... idk ui_debugger_.BindCallbacks() looks better
    debug::UiDebugger::BindCallbacks();
    ui_debugger_.ApplyAndReset();
    ui_debugger_.Update();
  } else if (debug_ui_prev_) {
    cur_mode_->BindDefaultCallbacks();
    ui_debugger_.ApplyAndReset();
    ui_debugger_.Reset();
  }
  debug_ui_prev_ = debug::gUiAltMode;
#endif  // NDEBUG

  ui_grid_.RenderAxis(1.0f);
  ui_grid_.RenderGrid();
  ui_grid_.RenderBoundary();

  cur_mode_->Render();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_.Bind();

  ui_compass_.Render();

  bool old_render_menu = render_menu_;
  render_menu_ = (glfwGetKey(gWindow, GLFW_KEY_TAB) == GLFW_PRESS);
  //  menu_.Render(render_menu_);
  if (old_render_menu != render_menu_) {
    if (render_menu_) {
      ui_menu_.BindCallbacks();
      ui_menu_.Show();
      ui_settings_.Show();
    } else {
      ui_menu_.Hide();
      ui_settings_.Hide();
      cur_mode_->BindDefaultCallbacks();
    }
  }
  if (text_renderer_.InputInProgress()) {
    text_renderer_.RenderInput();
  }
  ui_buttons_.Render();
}

void UiRenderer::RenderPicking() {
  cur_mode_->RenderPickingWorld();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.shader_sp_picking_.Bind();
  ui_compass_.RenderPicking();

  cur_mode_->RenderPicking();

  // because mode changes vao (3d model render)
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.shader_sp_picking_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_buttons_.RenderPicking();
}

void UiRenderer::Press(int key, int action) { ui_buttons_.Press(key, action); }

void UiRenderer::Parse() {
  // ui_menu_.Parse();
  terrain_.Parse();
  water_.Parse();
  biomes_.Parse();
  objects_.Parse();
  placement_.Parse();
  tiles_.Parse();
}

void UiRenderer::Serialize() {
  // ui_menu_.Serialize();
  terrain_.Serialize();
  water_.Serialize();
  biomes_.Serialize();
  objects_.Serialize();
  placement_.Serialize();
  tiles_.Serialize();
}

void UiRenderer::Init() {
  text_renderer_.PrerenderMenuText(
      static_cast<int>(data::TextId::kMenuTerrain),
      static_cast<int>(data::TextId::kFileOpen) + 1);
  ui_debugger_.ForceUpdate();
}

void UiRenderer::SetupGlobalData() {
  ui_shared_resources_.gltf_context_.windows = &windows_;
  ui_shared_resources_.gltf_context_.cur_mode = &cur_mode_;
  ui_shared_resources_.gltf_context_.menu = &ui_menu_;
  ui_shared_resources_.gltf_context_.ui_debugger = &ui_debugger_;
  ui_shared_resources_.gltf_context_.ui_shared_resources =
      &ui_shared_resources_;
  ui_shared_resources_.gltf_context_.text_renderer = &text_renderer_;
  ui_shared_resources_.gltf_context_.ui_renderer = this;
}

void UiRenderer::AskForConfirmation(data::TextId text_id,
                                    std::function<void()>&& callable) {
  ui_confirmation_.SetText(text_id);
  ui_confirmation_.SetCallable(std::move(callable));
  ui_confirmation_.Show();
}

void UiRenderer::RenderWorldOrigin(glm::vec4 position, glm::vec4 color) {
  ui_world_origin_.Render(position, color);
}

void UiRenderer::RenderAxis(float scale) { ui_grid_.RenderAxis(scale); }
