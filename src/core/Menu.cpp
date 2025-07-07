#include "Menu.h"

#include <glad/glad.h>

#include "../modes/UiSharedResources.h"
#include "../common/Vbos.h"
#include "../common/ShadersBinding.h"
#include "../core/TileRenderer.h"

Menu::Menu(
    UiSharedResources& ui_shared_resources,
    TextRenderer& text_renderer,
    IUiMode* terrain_mode,
    IUiMode* water_mode,
    IUiMode* roads_mode,
    IUiMode* fences_mode,
    IUiMode* placement_mode,
    IUiMode* objects_mode,
    IUiMode* biomes_mode,
    IUiMode* tiles_mode,
    IUiMode*& cur_mode)
    : ui_shared_resources_(ui_shared_resources),
      text_renderer_(text_renderer),
      modes_({terrain_mode, water_mode, roads_mode, fences_mode,
              placement_mode, objects_mode, biomes_mode, tiles_mode}),
      cur_mode_(cur_mode),
      /// text is first, then modes, then vision, then shaders (Menu::Press)
      /// TODO; more description
      /// first three are text, so not used as simple rendering, but only for text
      ui_tab_menu_(
          UiDynamicSprite{data::VboIdMain::kMenuDesk, data::TextId::kNotYet},
          1.0f,
          ui_shared_resources_,
          UiStaticSprite{data::VboIdMain::kMenuTerrain, data::TextId::kMenuTerrain,
                         [this]() {
                           this->SetMode(0);
                         }},
          UiStaticSprite{data::VboIdMain::kMenuWater, data::TextId::kMenuWater,
                         [this]() {
                           this->SetMode(1);
                         }},
          UiStaticSprite{data::VboIdMain::kMenuRoads, data::TextId::kMenuRoads,
                         [this]() {
                           this->SetMode(2);
                         }},
          UiStaticSprite{data::VboIdMain::kMenuFences, data::TextId::kMenuFences,
                         [this]() {
                           this->SetMode(3);
                         }},
          UiStaticSprite{data::VboIdMain::kMenuPlacement, data::TextId::kMenuPlacement,
                         [this]() {
                           this->SetMode(4);
                         }},
          UiStaticSprite{data::VboIdMain::kMenuObjects, data::TextId::kMenuObjects,
                         [this]() {
                           this->SetMode(5);
                         }},
          UiStaticSprite{data::VboIdMain::kMenuBiomes, data::TextId::kMenuBiomes,
                         [this]() {
                           this->SetMode(6);
                         }},
          UiStaticSprite{data::VboIdMain::kMenuTiles, data::TextId::kMenuTiles,
                         [this]() {
                           this->SetMode(7);
                         }},
          UiToggle{
              {data::VboIdMain::kMenuTerrainOff, data::TextId::kNotYet,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_ = !state;
               }},
              {data::VboIdMain::kMenuTerrainOn1, data::TextId::kNotYet},
              {data::VboIdMain::kMenuTerrainOn2, data::TextId::kNotYet},
              {data::VboIdMain::kMenuTerrainOn3, data::TextId::kNotYet}
          },
          UiToggle{
              {data::VboIdMain::kMenuWaterOff, data::TextId::kNotYet,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_water_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_water_ = !state;
               }},
              {data::VboIdMain::kMenuWaterOn1, data::TextId::kNotYet},
              {data::VboIdMain::kMenuWaterOn2, data::TextId::kNotYet},
              {data::VboIdMain::kMenuWaterOn3, data::TextId::kNotYet}
          },
          UiToggle{
              {data::VboIdMain::kMenuRoadsOff, data::TextId::kNotYet,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_roads_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_roads_ = !state;
               }},
              {data::VboIdMain::kMenuRoadsOn1, data::TextId::kNotYet},
              {data::VboIdMain::kMenuRoadsOn2, data::TextId::kNotYet},
              {data::VboIdMain::kMenuRoadsOn3, data::TextId::kNotYet}
          },
          UiToggle{
              {data::VboIdMain::kMenuFencesOff, data::TextId::kNotYet,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_fences_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_fences_ = !state;
               }},
              {data::VboIdMain::kMenuFencesOn1, data::TextId::kNotYet},
              {data::VboIdMain::kMenuFencesOn2, data::TextId::kNotYet},
              {data::VboIdMain::kMenuFencesOn3, data::TextId::kNotYet}
          },
          UiToggle{
              {data::VboIdMain::kMenuPlacementOff, data::TextId::kNotYet,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_placement_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_placement_ = !state;
               }},
              {data::VboIdMain::kMenuPlacementOn1, data::TextId::kNotYet},
              {data::VboIdMain::kMenuPlacementOn2, data::TextId::kNotYet},
              {data::VboIdMain::kMenuPlacementOn3, data::TextId::kNotYet}
          },
          UiToggle{
              {data::VboIdMain::kMenuObjectsOff, data::TextId::kNotYet,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_objects_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_objects_ = !state;
               }},
              {data::VboIdMain::kMenuObjectsOn1, data::TextId::kNotYet},
              {data::VboIdMain::kMenuObjectsOn2, data::TextId::kNotYet},
              {data::VboIdMain::kMenuObjectsOn3, data::TextId::kNotYet}
          },
          UiToggle{
              {data::VboIdMain::kMenuBiomesOff, data::TextId::kNotYet,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_biomes_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_biomes_ = !state;
               }},
              {data::VboIdMain::kMenuBiomesOn1, data::TextId::kNotYet},
              {data::VboIdMain::kMenuBiomesOn2, data::TextId::kNotYet},
              {data::VboIdMain::kMenuBiomesOn3, data::TextId::kNotYet}
          },
          UiToggle{
              {data::VboIdMain::kMenuTilesOff, data::TextId::kNotYet,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_tiles_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_tiles_ = !state;
               }},
              {data::VboIdMain::kMenuTilesOn1, data::TextId::kNotYet},
              {data::VboIdMain::kMenuTilesOn2, data::TextId::kNotYet},
              {data::VboIdMain::kMenuTilesOn3, data::TextId::kNotYet}
          },
          UiStaticSprite{data::VboIdMain::kMenuShaderWirebound,
                         data::TextId::kNotYet},
          UiToggle{
              {data::VboIdMain::kMenuShadersOff, data::TextId::kMenuTerrain,
               [this]() {
                 std::cout << "toggle shaders" << std::endl;
               }},
              {data::VboIdMain::kMenuShadersOn1, data::TextId::kMenuTerrain},
              {data::VboIdMain::kMenuShadersOn2, data::TextId::kMenuTerrain},
              {data::VboIdMain::kMenuShadersOn3, data::TextId::kMenuTerrain}
          },
          {data::VboIdMain::kMenuArrowSelect, data::TextId::kNotYet},
          {data::VboIdMain::kMenuArrowSelected, data::TextId::kNotYet},
          {data::VboIdMain::kMenuSave, data::TextId::kNotYet},
          {data::VboIdMain::kMenuLoad, data::TextId::kNotYet}),
      ui_settings_(
          UiDynamicSprite{data::VboIdMain::kSettingsDesk, data::TextId::kNotYet},
          4.0f,
          ui_shared_resources_,
          LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
          LocalTransform{glm::vec2{0.2f, 0.0f}, 1.0f, 0.0f},
          {
              {data::VboIdMain::kSettingsDeskPinBack, data::TextId::kNotYet, [](){}},
              {data::VboIdMain::kSettingsDeskPinPoint, data::TextId::kNotYet}},
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
          ) {}


void Menu::SetMode(int id) {
  cur_mode_ = modes_[id];
  arrow_selected_angle_ =
      static_cast<float>(id) * 2.0f * glm::pi<float>() / 8.0f;
  cur_mode_->BindCallbacks();
  auto text_ids = cur_mode_->GetPrerenderTextIds();
  text_renderer_.PrerenderModeText(text_ids.x, text_ids.y);
}

void Menu::Render(bool show) {
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();

  auto mouse_pos = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;

  if (show) {
    float angle = std::atan2(mouse_pos.y, mouse_pos.x) - glm::pi<float>() / 2.0f;
    ui_tab_menu_.Render(show, angle, arrow_selected_angle_);
    ui_settings_.Render();
  } else {
    Release();
  }
}

void Menu::RenderPicking() {
  ui_shared_resources_.static_sprite_picking_shader_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  /// first three for text (we don;t need picking for it)
  ui_tab_menu_.RenderPicking();
  ui_settings_.RenderPicking();
}

data::TextId Menu::Hover(uint32_t global_id) {
  auto hovered = ui_tab_menu_.Hover(global_id);
  if (hovered != data::TextId::kNone) {
    return hovered;
  }

  // TODO: arrow SELECT

  return ui_settings_.Hover(global_id);
}

bool Menu::Press(uint32_t global_id) {
  ui_tab_menu_.Press(global_id);
  ui_settings_.Press(global_id);
  return true;
}

void Menu::Release() {
  ui_tab_menu_.Release();
  ui_settings_.Release();
}

void Menu::Parse() {}

void Menu::Serialize() {}
