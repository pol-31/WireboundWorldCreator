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
          UiDynamicSprite{data::VboIdMain::kTabDesk, data::TextId::kNone},
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
              {data::VboIdMain::kMenuTerrainOff, data::TextId::kNone,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_ = !state;
               }},
              {data::VboIdMain::kMenuTerrainOn1, data::TextId::kNone},
              {data::VboIdMain::kMenuTerrainOn2, data::TextId::kNone},
              {data::VboIdMain::kMenuTerrainOn3, data::TextId::kNone}
          },
          UiToggle{
              {data::VboIdMain::kMenuWaterOff, data::TextId::kNone,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_water_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_water_ = !state;
               }},
              {data::VboIdMain::kMenuWaterOn1, data::TextId::kNone},
              {data::VboIdMain::kMenuWaterOn2, data::TextId::kNone},
              {data::VboIdMain::kMenuWaterOn3, data::TextId::kNone}
          },
          UiToggle{
              {data::VboIdMain::kMenuRoadsOff, data::TextId::kNone,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_roads_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_roads_ = !state;
               }},
              {data::VboIdMain::kMenuRoadsOn1, data::TextId::kNone},
              {data::VboIdMain::kMenuRoadsOn2, data::TextId::kNone},
              {data::VboIdMain::kMenuRoadsOn3, data::TextId::kNone}
          },
          UiToggle{
              {data::VboIdMain::kMenuFencesOff, data::TextId::kNone,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_fences_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_fences_ = !state;
               }},
              {data::VboIdMain::kMenuFencesOn1, data::TextId::kNone},
              {data::VboIdMain::kMenuFencesOn2, data::TextId::kNone},
              {data::VboIdMain::kMenuFencesOn3, data::TextId::kNone}
          },
          UiToggle{
              {data::VboIdMain::kMenuPlacementOff, data::TextId::kNone,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_placement_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_placement_ = !state;
               }},
              {data::VboIdMain::kMenuPlacementOn1, data::TextId::kNone},
              {data::VboIdMain::kMenuPlacementOn2, data::TextId::kNone},
              {data::VboIdMain::kMenuPlacementOn3, data::TextId::kNone}
          },
          UiToggle{
              {data::VboIdMain::kMenuObjectsOff, data::TextId::kNone,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_objects_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_objects_ = !state;
               }},
              {data::VboIdMain::kMenuObjectsOn1, data::TextId::kNone},
              {data::VboIdMain::kMenuObjectsOn2, data::TextId::kNone},
              {data::VboIdMain::kMenuObjectsOn3, data::TextId::kNone}
          },
          UiToggle{
              {data::VboIdMain::kMenuBiomesOff, data::TextId::kNone,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_biomes_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_biomes_ = !state;
               }},
              {data::VboIdMain::kMenuBiomesOn1, data::TextId::kNone},
              {data::VboIdMain::kMenuBiomesOn2, data::TextId::kNone},
              {data::VboIdMain::kMenuBiomesOn3, data::TextId::kNone}
          },
          UiToggle{
              {data::VboIdMain::kMenuTilesOff, data::TextId::kNone,
               [this]() {
                 bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_tiles_;
                 this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_tiles_ = !state;
               }},
              {data::VboIdMain::kMenuTilesOn1, data::TextId::kNone},
              {data::VboIdMain::kMenuTilesOn2, data::TextId::kNone},
              {data::VboIdMain::kMenuTilesOn3, data::TextId::kNone}
          },
          UiStaticSprite{data::VboIdMain::kMenuSettings, data::TextId::kNone,
                         [this]() {
                           this->show_settings_ = true;
                         }},
          UiStaticSprite{data::VboIdMain::kMenuShaderWirebound,
                         data::TextId::kNone},
          UiToggle{
              {data::VboIdMain::kMenuShadersOff, data::TextId::kMenuTerrain,
               [this]() {
                 std::cout << "toggle shaders" << std::endl;
               }},
              {data::VboIdMain::kMenuShadersOn1, data::TextId::kMenuTerrain},
              {data::VboIdMain::kMenuShadersOn2, data::TextId::kMenuTerrain},
              {data::VboIdMain::kMenuShadersOn3, data::TextId::kMenuTerrain}
          },
          UiDynamicSprite{data::VboIdMain::kMenuCross, data::TextId::kNone}),
      ui_settings_(
          UiDynamicSprite{data::VboIdMain::kConfigDesk, data::TextId::kNone},
          4.0f,
          ui_shared_resources_,
          UiSliderH3{
              {data::VboIdMain::kSettingsResolutionFill, data::TextId::kNone},
              {data::VboIdMain::kSettingsResolutionBack, data::TextId::kNone},
              {data::VboIdMain::kSettingsResolutionIcon, data::TextId::kNone},
          },
          UiSliderH3{
              {data::VboIdMain::kSettingsSoundFill, data::TextId::kNone},
              {data::VboIdMain::kSettingsSoundBack, data::TextId::kNone},
              {data::VboIdMain::kSettingsSoundIcon, data::TextId::kNone},
          },
          UiSliderH3{
              {data::VboIdMain::kSettingsMusicFill, data::TextId::kNone},
              {data::VboIdMain::kSettingsMusicBack, data::TextId::kNone},
              {data::VboIdMain::kSettingsMusicIcon, data::TextId::kNone},
          },
          UiSliderH3{
              {data::VboIdMain::kSettingsSensitivityFill, data::TextId::kNone},
              {data::VboIdMain::kSettingsSensitivityBack, data::TextId::kNone},
              {data::VboIdMain::kSettingsSensitivityIcon, data::TextId::kNone},
          },
          UiStaticSprite{data::VboIdMain::kSettingsKeyboard, data::TextId::kNone},
          UiToggle{
              {data::VboIdMain::kSettingsMusicOff, data::TextId::kNone,
               [this]() {
                 std::cout << "toggle music" << std::endl;
               }},
              {data::VboIdMain::kSettingsMusicOn1, data::TextId::kNone},
              {data::VboIdMain::kSettingsMusicOn2, data::TextId::kNone},
              {data::VboIdMain::kSettingsMusicOn3, data::TextId::kNone}
          },
          UiToggle{
              {data::VboIdMain::kSettingsSoundOff, data::TextId::kNone,
               [this]() {
                 std::cout << "toggle sound" << std::endl;
               }},
              {data::VboIdMain::kSettingsSoundOn1, data::TextId::kNone},
              {data::VboIdMain::kSettingsSoundOn2, data::TextId::kNone},
              {data::VboIdMain::kSettingsSoundOn3, data::TextId::kNone}
          },
          UiDynamicSprite{data::VboIdMain::kSettingsCross, data::TextId::kNone}) {}


void Menu::SetMode(int id) {
  cur_mode_ = modes_[id];
  cur_mode_->BindCallbacks();
  auto text_ids = cur_mode_->GetPrerenderTextIds();
  text_renderer_.PrerenderModeText(text_ids.x, text_ids.y);
}

void Menu::Render(bool show) {
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();

  ui_shared_resources_.tex_ui_.Bind();

  if (!show) {
    Release();
    show_settings_ = false;
  }

  if (show_settings_) {
    ui_settings_.Render(show);
  } else {
    ui_tab_menu_.Render(show);
  }
}

void Menu::RenderPicking() {
  ui_shared_resources_.static_sprite_picking_shader_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  /// first three for text (we don;t need picking for it)
  ui_tab_menu_.RenderPicking();
  if (show_settings_) {
    ui_settings_.RenderPicking();
  }
}

data::TextId Menu::Hover(uint32_t global_id) {
  if (!show_settings_) {
    return ui_tab_menu_.Hover(global_id);
  } else {
    return ui_settings_.Hover(global_id);
  }
}

bool Menu::Press(uint32_t global_id) {
  if (!show_settings_) {
    ui_tab_menu_.Press(global_id);
  } else {
    ui_settings_.Press(global_id);
  }
  return true;
}

void Menu::Release() {
  if (!show_settings_) {
    ui_tab_menu_.Release();
  } else {
    ui_settings_.Release();
  }
}

void Menu::Parse() {}

void Menu::Serialize() {}
