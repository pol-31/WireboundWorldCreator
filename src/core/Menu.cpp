#include "Menu.h"

#include <glad/glad.h>

#include "../modes/SharedResources.h"
#include "../common/Vbos.h"
#include "../common/ShadersBinding.h"

Menu::Menu(SharedResources& shared_resources,
           const TextRenderer& text_renderer,
           Visibility& visibility,
           IEditMode* terrain_mode,
           IEditMode* water_mode,
           IEditMode* roads_mode,
           IEditMode* fences_mode,
           IEditMode* placement_mode,
           IEditMode* objects_mode,
           IEditMode* biomes_mode,
           IEditMode* tiles_mode,
           IEditMode*& cur_mode)
    : shared_resources_(shared_resources),
      text_renderer_(text_renderer),
      modes_({terrain_mode, water_mode, roads_mode, fences_mode,
              placement_mode, objects_mode, biomes_mode, tiles_mode}),
      cur_mode_(cur_mode),
      visibility_(visibility),
      /// text is first, then modes, then vision, then shaders (Menu::Press)
      /// TODO; more description
      /// first three are text, so not used as simple rendering, but only for text
      ui_tab_menu_(
          UiStaticSprite{vbos::VboIdMain::kTabDesk, vbos::VboIdText::kNone},
          shared_resources_,
          UiStaticSprite{vbos::VboIdMain::kMenuTerrain, vbos::VboIdText::kTerrain,
                         [this]() {
                           this->BtnModeTerrain();
                         }},
          UiStaticSprite{vbos::VboIdMain::kMenuWater, vbos::VboIdText::kWater,
                         [this]() {
                           this->BtnModeWater();
                         }},
          UiStaticSprite{vbos::VboIdMain::kMenuRoads, vbos::VboIdText::kRoads,
                         [this]() {
                           this->BtnModeRoads();
                         }},
          UiStaticSprite{vbos::VboIdMain::kMenuFences, vbos::VboIdText::kFences,
                         [this]() {
                           this->BtnModeFences();
                         }},
          UiStaticSprite{vbos::VboIdMain::kMenuPlacement, vbos::VboIdText::kPlacement,
                         [this]() {
                           this->BtnModePlacement();
                         }},
          UiStaticSprite{vbos::VboIdMain::kMenuObjects, vbos::VboIdText::kObject,
                         [this]() {
                           this->BtnModeObjects();
                         }},
          UiStaticSprite{vbos::VboIdMain::kMenuBiomes, vbos::VboIdText::kBiome,
                         [this]() {
                           this->BtnModeBiomes();
                         }},
          UiStaticSprite{vbos::VboIdMain::kMenuTiles, vbos::VboIdText::kTiles,
                         [this]() {
                           this->BtnModeTiles();
                         }},
          UiToggle{
              {vbos::VboIdMain::kMenuTerrainOff, vbos::VboIdText::kNone,
               [this]() {
                 this->ToggleTerrain();
               }},
              {vbos::VboIdMain::kMenuTerrainOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuTerrainOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuTerrainOn3, vbos::VboIdText::kNone}
          },
          UiToggle{
              {vbos::VboIdMain::kMenuWaterOff, vbos::VboIdText::kNone,
               [this]() {
                 this->ToggleWater();
               }},
              {vbos::VboIdMain::kMenuWaterOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuWaterOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuWaterOn3, vbos::VboIdText::kNone}
          },
          UiToggle{
              {vbos::VboIdMain::kMenuRoadsOff, vbos::VboIdText::kNone,
               [this]() {
                 this->ToggleRoads();
               }},
              {vbos::VboIdMain::kMenuRoadsOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuRoadsOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuRoadsOn3, vbos::VboIdText::kNone}
          },
          UiToggle{
              {vbos::VboIdMain::kMenuFencesOff, vbos::VboIdText::kNone,
               [this]() {
                 this->ToggleFences();
               }},
              {vbos::VboIdMain::kMenuFencesOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuFencesOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuFencesOn3, vbos::VboIdText::kNone}
          },
          UiToggle{
              {vbos::VboIdMain::kMenuPlacementOff, vbos::VboIdText::kNone,
               [this]() {
                 this->TogglePlacement();
               }},
              {vbos::VboIdMain::kMenuPlacementOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuPlacementOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuPlacementOn3, vbos::VboIdText::kNone}
          },
          UiToggle{
              {vbos::VboIdMain::kMenuObjectsOff, vbos::VboIdText::kNone,
               [this]() {
                 this->ToggleObjects();
               }},
              {vbos::VboIdMain::kMenuObjectsOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuObjectsOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuObjectsOn3, vbos::VboIdText::kNone}
          },
          UiToggle{
              {vbos::VboIdMain::kMenuBiomesOff, vbos::VboIdText::kNone,
               [this]() {
                 this->ToggleBiomes();
               }},
              {vbos::VboIdMain::kMenuBiomesOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuBiomesOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuBiomesOn3, vbos::VboIdText::kNone}
          },
          UiToggle{
              {vbos::VboIdMain::kMenuTilesOff, vbos::VboIdText::kNone,
               [this]() {
                 this->ToggleTiles();
               }},
              {vbos::VboIdMain::kMenuTilesOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuTilesOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kMenuTilesOn3, vbos::VboIdText::kNone}
          },
          UiStaticSprite{vbos::VboIdMain::kMenuSettings, vbos::VboIdText::kNone,
                         [this]() {
                           this->ShowSettings();
                         }},
          UiStaticSprite{vbos::VboIdMain::kMenuShaderWirebound, vbos::VboIdText::kWirebound},
          UiToggle{
              {vbos::VboIdMain::kMenuShadersOff, vbos::VboIdText::kTerrain,
               [this]() {
                 this->ToggleShaders();
               }},
              {vbos::VboIdMain::kMenuShadersOn1, vbos::VboIdText::kTerrain},
              {vbos::VboIdMain::kMenuShadersOn2, vbos::VboIdText::kTerrain},
              {vbos::VboIdMain::kMenuShadersOn3, vbos::VboIdText::kTerrain}
          },
          UiDynamicSprite{vbos::VboIdMain::kCross, vbos::VboIdText::kNone}),
      ui_settings_(
          UiStaticSprite{vbos::VboIdMain::kConfigDesk, vbos::VboIdText::kNone},
          shared_resources_,
          UiSliderH{
              {vbos::VboIdMain::kSettingsResolutionFill, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsResolutionBack, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsResolutionIcon, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsResolutionHandler, vbos::VboIdText::kNone},
          },
          UiSliderH{
              {vbos::VboIdMain::kSettingsSoundFill, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsSoundBack, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsSoundIcon, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsSoundHandler, vbos::VboIdText::kNone},
          },
          UiSliderH{
              {vbos::VboIdMain::kSettingsMusicFill, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsMusicBack, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsMusicIcon, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsMusicHandler, vbos::VboIdText::kNone},
          },
          UiSliderH{
              {vbos::VboIdMain::kSettingsSensitivityFill, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsSensitivityBack, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsSensitivityIcon, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsSensitivityHandler, vbos::VboIdText::kNone},
          },
          UiStaticSprite{vbos::VboIdMain::kSettingsKeyboard, vbos::VboIdText::kNone},
          UiToggle{
              {vbos::VboIdMain::kSettingsMusicOff, vbos::VboIdText::kNone,
               [this]() {
                 this->ToggleMusic();
               }},
              {vbos::VboIdMain::kSettingsMusicOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsMusicOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsMusicOn3, vbos::VboIdText::kNone}
          },
          UiToggle{
              {vbos::VboIdMain::kSettingsSoundOff, vbos::VboIdText::kNone,
               [this]() {
                 this->ToggleSound();
               }},
              {vbos::VboIdMain::kSettingsSoundOn1, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsSoundOn2, vbos::VboIdText::kNone},
              {vbos::VboIdMain::kSettingsSoundOn3, vbos::VboIdText::kNone}
          },
          UiDynamicSprite{vbos::VboIdMain::kObjectsVodyaniy, vbos::VboIdText::kNone}
      ) {
  Init();
}

void Menu::Init() {
  /// first is terrain mode
  selected_mode_idx_ = 3;
  InitText();
}

void Menu::InitText() {
  for (int i = 0; i < 3; ++i) {
    auto text_offset = 0;//ui_components_.widgets_[i]->GetTextVboOffset();
    float width = vbos::kUiVboDataText[text_offset * 2]
                  - vbos::kUiVboDataText[text_offset * 2 + 4];
    float height = vbos::kUiVboDataText[text_offset * 2 + 3]
                   - vbos::kUiVboDataText[text_offset * 2 + 1];
    // based on text sprite size
    text_params_[i].scale = glm::vec2{width, height};
  }
  /// "mode", "visibility", "shaders"
  text_params_[0].translate = glm::vec2{-0.6f, 0.05f};
  text_params_[1].translate = glm::vec2{-0.6f, -0.05f};
  text_params_[2].translate = glm::vec2{-0.6f, -0.15f};
}

void Menu::ShowSettings() {
  show_settings_ = true;
}

void Menu::HideSettings() {
  show_settings_ = false;
}

void Menu::Render(bool show) {
  text_renderer_.Bind();
  for (int i = 0; i < 3; ++i) {
//    glUniform2fv(shader::kTextTranslate, 1,
//                 glm::value_ptr(text_params_[i].translate));
//    glUniform2fv(shader::kTextScale, 1,
//                 glm::value_ptr(text_params_[i].scale));
//    glDrawArrays(GL_TRIANGLE_STRIP,
//                 static_cast<GLint>(ui_components_.widgets_[i]->GetTextVboOffset()), 4);
  }

  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader_.Bind();

  shared_resources_.tex_ui_.Bind();

  if (show_settings_) {
    ui_settings_.Render(show);
    if (!show) {
      HideSettings();
    }
    return;
  } else {
    ui_tab_menu_.Render(show);
  }
}

void Menu::RenderPicking() {
  shared_resources_.static_sprite_picking_shader_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);
  /// first three for text (we don;t need picking for it)
  ui_tab_menu_.RenderPicking();
  if (show_settings_) {
    ui_settings_.RenderPicking();
  }
}

int Menu::Hover(uint32_t global_id) {
  // skip first 3 for text
  if (global_id < static_cast<int>(vbos::VboIdMain::kMenuTerrain) ||
      global_id > static_cast<int>(vbos::VboIdMain::kMenuTerrain) + 16) {
    mode_hover_hover_ = 0.0f;
    return -1;
  }
  int local_id = static_cast<int>(global_id)
                 - static_cast<int>(vbos::VboIdMain::kMenuTerrain);

  if (local_id < 18 &&
      selected_mode_idx_ != local_id &&
      !(local_id > 7 && visibility_.visibility & (1 << (local_id - 8)))) {
    if (last_hovered_ != local_id) {
      mode_hover_hover_ = 0.0f;
    }
    mode_hover_hover_ = std::min(1.0f, mode_hover_hover_ + gDeltaTime * 0.8f);
    shared_resources_.menu_icon_shader_.Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(shared_resources_.vao_ui_);
    shared_resources_.tex_ui_.Bind();
    glUniform1f(4, mode_hover_hover_); // TODO: do we need it? <-------
//    ui_components_.widgets_[local_id]->Render(); // first 3 are text
    last_hovered_ = local_id;
  }
  return 0;//ui_components_.widgets_[local_id]->Hover();
}

bool Menu::Press(uint32_t global_id) {
  if (!show_settings_) {
    ui_tab_menu_.Press(global_id);
  } else {
    ui_settings_.Press(global_id);
  }
  return true;
}
