#include "UiMenu.h"

#include "../common/PickingFramebuffer.h"
#include "../modes/IUiMode.h"
#include "../renderers/UiRenderer.h"
#include "TileRenderer.h"

UiMenu::UiMenu(UiSharedResources& ui_shared_resources,
               TextRenderer& text_renderer, WindowQueue& window_queue,
               IUiMode* terrain_mode, IUiMode* water_mode,
               IUiMode* placement_mode, IUiMode* objects_mode,
               IUiMode* biomes_mode, IUiMode* tiles_mode, IUiMode* player_mode,
               IUiMode*& cur_mode)
    : UiWindowAppear({data::VboIdMain::kMenuDesk}, 1.0f,
                     {{data::VboIdMain::kMenuDeskPinBack, []() {}},
                      {data::VboIdMain::kMenuDeskPinPoint}},
                     ui_shared_resources, window_queue),
      modes_({terrain_mode, water_mode, placement_mode, objects_mode,
              biomes_mode, tiles_mode, player_mode}),
      cur_mode_(cur_mode),
      btn_terrain_{data::VboIdMain::kMenuTerrain,
                   [this]() { this->SetMode(0); }},
      btn_water_{data::VboIdMain::kMenuWater, [this]() { this->SetMode(1); }},
      btn_placement_{data::VboIdMain::kMenuPlacement,
                     [this]() { this->SetMode(2); }},
      btn_objects_{data::VboIdMain::kMenuObjects,
                   [this]() { this->SetMode(3); }},
      btn_biomes_{data::VboIdMain::kMenuBiomes, [this]() { this->SetMode(4); }},
      btn_tiles_{data::VboIdMain::kMenuTiles, [this]() { this->SetMode(5); }},
      btn_player_{data::VboIdMain::kMenuPlayer, [this]() { this->SetMode(6); }},

      tg_terrain_{{data::VboIdMain::kMenuTerrainOff,
                   [this]() {
                     bool state = this->ui_shared_resources_.glfw_context_
                                      .tile_renderer->show_terrain_;
                     this->ui_shared_resources_.glfw_context_.tile_renderer
                         ->show_terrain_ = !state;
                   }},
                  {data::VboIdMain::kMenuTerrainOn1},
                  {data::VboIdMain::kMenuTerrainOn2},
                  {data::VboIdMain::kMenuTerrainOn3}},
      tg_water_{{data::VboIdMain::kMenuWaterOff,
                 [this]() {
                   bool state = this->ui_shared_resources_.glfw_context_
                                    .tile_renderer->show_water_;
                   this->ui_shared_resources_.glfw_context_.tile_renderer
                       ->show_water_ = !state;
                 }},
                {data::VboIdMain::kMenuWaterOn1},
                {data::VboIdMain::kMenuWaterOn2},
                {data::VboIdMain::kMenuWaterOn3}},
      tg_placement_{{data::VboIdMain::kMenuPlacementOff,
                     [this]() {
                       bool state = this->ui_shared_resources_.glfw_context_
                                        .tile_renderer->show_placement_;
                       this->ui_shared_resources_.glfw_context_.tile_renderer
                           ->show_placement_ = !state;
                     }},
                    {data::VboIdMain::kMenuPlacementOn1},
                    {data::VboIdMain::kMenuPlacementOn2},
                    {data::VboIdMain::kMenuPlacementOn3}},
      tg_objects_{{data::VboIdMain::kMenuObjectsOff,
                   [this]() {
                     bool state = this->ui_shared_resources_.glfw_context_
                                      .tile_renderer->show_objects_;
                     this->ui_shared_resources_.glfw_context_.tile_renderer
                         ->show_objects_ = !state;
                   }},
                  {data::VboIdMain::kMenuObjectsOn1},
                  {data::VboIdMain::kMenuObjectsOn2},
                  {data::VboIdMain::kMenuObjectsOn3}},
      tg_biomes_{{data::VboIdMain::kMenuBiomesOff,
                  [this]() {
                    bool state = this->ui_shared_resources_.glfw_context_
                                     .tile_renderer->show_biomes_;
                    this->ui_shared_resources_.glfw_context_.tile_renderer
                        ->show_biomes_ = !state;
                  }},
                 {data::VboIdMain::kMenuBiomesOn1},
                 {data::VboIdMain::kMenuBiomesOn2},
                 {data::VboIdMain::kMenuBiomesOn3}},
      tg_tiles_{{data::VboIdMain::kMenuTilesOff,
                 [this]() {
                   bool state = this->ui_shared_resources_.glfw_context_
                                    .tile_renderer->show_tiles_;
                   this->ui_shared_resources_.glfw_context_.tile_renderer
                       ->show_tiles_ = !state;
                 }},
                {data::VboIdMain::kMenuTilesOn1},
                {data::VboIdMain::kMenuTilesOn2},
                {data::VboIdMain::kMenuTilesOn3}},

      btn_shader_wirebound_{data::VboIdMain::kMenuShaderWirebound},
      toggle_shaders_{
          {data::VboIdMain::kMenuShadersOff,
           [this]() { std::cout << "toggle shaders" << std::endl; }},
          {data::VboIdMain::kMenuShadersOn1},
          {data::VboIdMain::kMenuShadersOn2},
          {data::VboIdMain::kMenuShadersOn3}},

      arrow_select_{data::VboIdMain::kMenuArrowSelect},
      arrow_selected_{data::VboIdMain::kMenuArrowSelected},
      save_data_{
          data::VboIdMain::kMenuSave,
          [this] {
            this->ui_shared_resources_.glfw_context_.ui_renderer->Serialize();
          }},
      load_data_{
          data::VboIdMain::kMenuLoad,
          [this] {
            this->ui_shared_resources_.glfw_context_.ui_renderer->Parse();
          }},
      text_filename_(text_renderer, {data::VboIdMain::kFileTextLabel},
                     {data::VboIdMain::kFileTextBack}),
      txt_mode_{text_renderer,
                {data::VboIdMain::kModeModeText},
                data::TextId::kMenuTerrain},
      ui_event_handler_({
          &pin_, &btn_terrain_, &btn_water_, &btn_placement_, &btn_objects_,
          &btn_biomes_, &btn_tiles_, &btn_player_, &tg_terrain_, &tg_water_,
          &tg_placement_, &tg_objects_, &tg_biomes_, &tg_tiles_,
          &btn_shader_wirebound_, &toggle_shaders_, &save_data_,
          &load_data_ /*, &text_filename_*/
      }),
      hierarchy_(&background_, {&pin_,
                                &btn_terrain_,
                                &btn_water_,
                                &btn_placement_,
                                &btn_objects_,
                                &btn_biomes_,
                                &btn_tiles_,
                                &btn_player_,
                                &tg_terrain_,
                                &tg_water_,
                                &tg_placement_,
                                &tg_objects_,
                                &tg_biomes_,
                                &tg_tiles_,
                                &btn_shader_wirebound_,
                                &toggle_shaders_,
                                &arrow_select_,
                                &arrow_selected_,
                                &save_data_,
                                &load_data_,
                                &text_filename_}) {
  speed_ = 2.0f;
  SetCircleTransform();
}

void UiMenu::SetMode(int id) {
  float angle =
      static_cast<float>(id) * 2.0f * glm::pi<float>() / modes_.size();
  arrow_selected_angle_ = angle;
  txt_mode_.SetText(static_cast<data::TextId>(
      id + static_cast<int>(data::TextId::kMenuTerrain)));
  cur_mode_ = modes_[id];
  cur_mode_->Setup();
}

void UiMenu::BindCallbacks() {
  glfwSetScrollCallback(gWindow, menu::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, menu::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, nullptr);
}

// returns "stop render"
bool UiMenu::Render() {
  ui_shared_resources_.tex_ui_.Bind();
  bool stop_show = Base::RenderBack(true);
  //  if (!Base::BackIsReady()) {
  //    return stop_show;
  //  }

  ui_shared_resources_.shader_sp_.Bind();

  btn_terrain_.Render();
  btn_water_.Render();
  btn_placement_.Render();
  btn_objects_.Render();
  btn_biomes_.Render();
  btn_tiles_.Render();
  btn_player_.Render();

  tg_terrain_.Render();
  tg_water_.Render();
  tg_placement_.Render();
  tg_objects_.Render();
  tg_biomes_.Render();
  tg_tiles_.Render();

  btn_shader_wirebound_.Render();

  toggle_shaders_.Render();

  ui_shared_resources_.shader_sp_.Bind();

  auto cursor_pos = ui_shared_resources_.glfw_context_.cursor_pos_tex_norm_;
  float arrow_select_angle =
      std::atan2(-cursor_pos.x, cursor_pos.y * gResFactor);

  arrow_select_.SetRotate(arrow_select_angle);
  arrow_selected_.SetRotate(arrow_selected_angle_);

  arrow_select_.Render();
  arrow_selected_.Render();
  save_data_.Render();
  load_data_.Render();
  text_filename_.Render();
  txt_mode_.Render();

  return stop_show;
}

void UiMenu::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  ui_shared_resources_.shader_sp_picking_.Bind();

  btn_terrain_.RenderPicking();
  btn_water_.RenderPicking();
  btn_placement_.RenderPicking();
  btn_objects_.RenderPicking();
  btn_biomes_.RenderPicking();
  btn_tiles_.RenderPicking();
  btn_player_.RenderPicking();

  tg_terrain_.RenderPicking();
  tg_water_.RenderPicking();
  tg_placement_.RenderPicking();
  tg_objects_.RenderPicking();
  tg_biomes_.RenderPicking();
  tg_tiles_.RenderPicking();

  btn_shader_wirebound_.RenderPicking();

  toggle_shaders_.RenderPicking();

  ui_shared_resources_.shader_sp_picking_.Bind();

  arrow_select_.RenderPicking();
  arrow_selected_.RenderPicking();
  save_data_.RenderPicking();
  load_data_.RenderPicking();
  text_filename_.RenderPicking();
  txt_mode_.RenderPicking();
}

bool UiMenu::Press(int id) {
  bool handled = ui_event_handler_.Press(id);
  if (id != pin_.GetId()) {
    return handled;
  }
  if (!do_show_) {
    Hide();
  }
  return handled;
}

void UiMenu::Release() { ui_event_handler_.Release(); }

void UiMenu::SetCircleTransform() {
  const float radius_mode = 0.5f;
  const int total_modes = 7;
  const float radius_checkbox = 0.65f;
  SetSpTransform(btn_terrain_, 0, total_modes, radius_mode);
  SetSpTransform(btn_water_, 1, total_modes, radius_mode);
  SetSpTransform(btn_placement_, 2, total_modes, radius_mode);
  SetSpTransform(btn_objects_, 3, total_modes, radius_mode);
  SetSpTransform(btn_biomes_, 4, total_modes, radius_mode);
  SetSpTransform(btn_tiles_, 5, total_modes, radius_mode);
  SetSpTransform(btn_player_, 6, total_modes, radius_mode);
  SetTg4Transform(tg_terrain_, 0, total_modes, radius_checkbox);
  SetTg4Transform(tg_water_, 1, total_modes, radius_checkbox);
  SetTg4Transform(tg_placement_, 2, total_modes, radius_checkbox);
  SetTg4Transform(tg_objects_, 3, total_modes, radius_checkbox);
  SetTg4Transform(tg_biomes_, 4, total_modes, radius_checkbox);
  SetTg4Transform(tg_tiles_, 5, total_modes, radius_checkbox);
}

void UiMenu::SetSpTransform(UiSprite& sp, int id, int total_num, float radius) {
  float angle =
      (2.0f * glm::pi<float>() / total_num) * id + glm::half_pi<float>();
  glm::vec2 translate = {cos(angle) * radius, sin(angle) * radius};
  translate *= glm::vec2{gResFactor, 1.0f};
  LocalTransform transform;
  transform.translate = translate;
  sp.SetParentTransform(transform);
}

void UiMenu::SetTg4Transform(UiToggle4& tg, int id, int total_num,
                             float radius) {
  float angle =
      (2.0f * glm::pi<float>() / total_num) * id + glm::half_pi<float>();
  glm::vec2 translate = {cos(angle) * radius, sin(angle) * radius};
  translate *= glm::vec2{gResFactor, 1.0f};
  LocalTransform transform;
  transform.scale = 0.6f;
  transform.translate = translate;
  tg.SetParentTransform(transform);
}

namespace menu {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto glfw_context = GetGlfwContext(window);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    glfw_context->windows->Scroll(pressed_id, yoffset);
  }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto glfw_context = GetGlfwContext(window);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    glfw_context->windows->Press(pressed_id);
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    glfw_context->windows->Release();
  }
}

}  // namespace menu
