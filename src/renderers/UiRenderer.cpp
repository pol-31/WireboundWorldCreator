#include "UiRenderer.h"

#include "../io/Camera.h"

UiRenderer::UiRenderer(GlfwContext& global_glfw_data_,
                       TileRenderer& tile_renderer, const Camera* camera)
    : render_data_(global_glfw_data_),
      mdl_manager_(render_data_),
      ui_debugger_(render_data_.vbo_ui_,
                   render_data_.vbo_ui_transform_,
                   global_glfw_data_.cursor_pos_tex_norm_),
      text_renderer_(render_data_,
                     {data::VboIdMain::kTextRendererPrerenderSlot},
                     {data::VboIdMain::kTextRendererCursor}),
      ui_slots_(render_data_, text_renderer_),
      ui_config_window_(render_data_, windows_, text_renderer_),
      ui_edit_slots_(render_data_, windows_, text_renderer_),
      ui_edit_1_(render_data_, text_renderer_),
      ui_edit_2_(render_data_, text_renderer_),
      terrain_(render_data_, ui_slots_, windows_, text_renderer_,
               ui_edit_slots_, ui_edit_1_, tile_renderer.cur_tile_,
               ui_config_window_),
      water_(render_data_, ui_slots_, windows_, text_renderer_,
             ui_edit_slots_, ui_edit_1_, ui_edit_2_, ui_config_window_,
             mdl_manager_),
      biomes_(render_data_, ui_slots_, windows_, text_renderer_,
              ui_edit_slots_, ui_edit_2_, mdl_manager_),
      objects_(render_data_, ui_slots_, windows_, text_renderer_,
               ui_edit_slots_, ui_edit_2_, mdl_manager_),
      placement_(render_data_, ui_slots_, windows_, ui_edit_slots_,
                 ui_edit_2_, mdl_manager_),
      tiles_(render_data_, windows_),
      player_(render_data_, windows_, text_renderer_,
              tile_renderer.cur_tile_, mdl_manager_),
      ui_menu_(render_data_, text_renderer_, windows_, &terrain_,
               &water_, &placement_, &objects_, &biomes_, &tiles_, &player_,
               cur_mode_),
      ui_settings_({data::VboIdMain::kSettingsDesk}, 2.0f,
                   {{data::VboIdMain::kSettingsDeskPinBack, []() {}},
                    {data::VboIdMain::kSettingsDeskPinPoint}},
                   render_data_, windows_,
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
      ui_confirmation_(render_data_, text_renderer_),
      ui_warning_(render_data_, text_renderer_),
      ui_tip_({data::VboIdMain::kTipInfoDesk}, 1.0f,
              {
                  {data::VboIdMain::kTipInfoDeskPinBack, []() {}},
                  {data::VboIdMain::kTipInfoDeskPinPoint},
              },
              render_data_, windows_, {data::VboIdMain::kTipInfoText}),
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
      ui_layer_wireframe_(render_data_),
      ui_world_origin_(render_data_),
      ui_buttons_(render_data_, text_renderer_),
      ui_grid_(render_data_) {
  ui_edit_1_.AttachToHierarchy(ui_edit_slots_.hierarchy_);
  ui_edit_2_.AttachToHierarchy(ui_edit_slots_.hierarchy_);
  Init();
}

void UiRenderer::Render(TileRenderer* tile_renderer) {
  cur_mode_->Render(tile_renderer, this);
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

  cur_mode_->Render(tile_renderer, this);
  windows_.Render();
  render_data_.tex_ui_.BindSampler(0);
  glBindVertexArray(render_data_.vao_ui_);
  render_data_.shader_sp_.Bind();

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
  ui_confirmation_.Render();
  ui_warning_.Render();
}

void UiRenderer::RenderPicking(TileRenderer* tile_renderer) {
  ui_confirmation_.RenderPicking();
  ui_warning_.RenderPicking();
  if (ui_confirmation_.IsActive() || ui_warning_.IsActive()) {
    return;
  }

  cur_mode_->RenderPicking(tile_renderer, this);
  windows_.RenderPicking();

  glBindVertexArray(render_data_.vao_ui_);

  render_data_.tex_ui_.BindSampler(0);
  render_data_.shader_sp_picking_.Bind();
  ui_compass_.RenderPicking();

  render_data_.tex_ui_.BindSampler(0);
  render_data_.shader_sp_picking_.Bind();
  glBindVertexArray(render_data_.vao_ui_);

  ui_buttons_.RenderPicking();
}

void UiRenderer::Press(int key, int action) { ui_buttons_.Press(key, action); }

void UiRenderer::Parse() {
  // TODO: GenerateAll() for all (maybe)
  render_data_.glfw_context_.tile_renderer->cur_tile_.Parse("");
  terrain_.ui_edit_.GenerateAll();
  water_.ui_edit_river_.Generate();
  water_.ui_edit_ocean_.GenerateAll();
  biomes_.ui_edit_.Generate();
  objects_.ui_edit_.Generate();
  placement_.ui_edit_.Generate();
  render_data_.glfw_context_.tile_renderer->UpdatePipeline();
  terrain_.ui_bake_.Bake(1000.0f, 1000.0f, 0.1f);
}

void UiRenderer::Serialize() {
  render_data_.glfw_context_.tile_renderer->cur_tile_.Serialize("");
}

void UiRenderer::Init() {
  text_renderer_.PrerenderMenuText(
      static_cast<int>(data::TextId::kMenuTerrain),
      static_cast<int>(data::TextId::kFileOpen) + 1);
  ui_debugger_.ForceUpdate();
}

void UiRenderer::SetupGlobalData() {
  render_data_.glfw_context_.windows = &windows_;
  render_data_.glfw_context_.cur_mode = &cur_mode_;
  render_data_.glfw_context_.menu = &ui_menu_;
  render_data_.glfw_context_.ui_debugger = &ui_debugger_;
  render_data_.glfw_context_.render_data =
      &render_data_;
  render_data_.glfw_context_.text_renderer = &text_renderer_;
  render_data_.glfw_context_.ui_confirmation = &ui_confirmation_;
  render_data_.glfw_context_.ui_warning = &ui_warning_;
  render_data_.glfw_context_.text_renderer = &text_renderer_;
  render_data_.glfw_context_.ui_renderer = this;
}

void UiRenderer::RenderWorldOrigin(glm::vec4 position, glm::vec4 color) {
  ui_world_origin_.Render(position, color);
}

void UiRenderer::RenderAxis(float scale) { ui_grid_.RenderAxis(scale); }
