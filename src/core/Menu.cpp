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

          UiStaticSprite{vbos::VboIdMain::kMenuTerrain, vbos::VboIdText::kTerrain},
          UiStaticSprite{vbos::VboIdMain::kMenuWater, vbos::VboIdText::kWater},
          UiStaticSprite{vbos::VboIdMain::kMenuRoads, vbos::VboIdText::kRoads},
          UiStaticSprite{vbos::VboIdMain::kMenuFences, vbos::VboIdText::kFences},
          UiStaticSprite{vbos::VboIdMain::kMenuPlacement, vbos::VboIdText::kPlacement},
          UiStaticSprite{vbos::VboIdMain::kMenuObjects, vbos::VboIdText::kObject},
          UiStaticSprite{vbos::VboIdMain::kMenuBiomes, vbos::VboIdText::kBiome},
          UiStaticSprite{vbos::VboIdMain::kMenuTiles, vbos::VboIdText::kTiles},

          UiStaticSprite{vbos::VboIdMain::kMenuTerrainOn, vbos::VboIdText::kTerrain},
          UiStaticSprite{vbos::VboIdMain::kMenuWaterOn, vbos::VboIdText::kWater},
          UiStaticSprite{vbos::VboIdMain::kMenuRoadsOn, vbos::VboIdText::kRoads},
          UiStaticSprite{vbos::VboIdMain::kMenuFencesOn, vbos::VboIdText::kFences},
          UiStaticSprite{vbos::VboIdMain::kMenuPlacementOn, vbos::VboIdText::kPlacement},
          UiStaticSprite{vbos::VboIdMain::kMenuObjectsOn, vbos::VboIdText::kObject},
          UiStaticSprite{vbos::VboIdMain::kMenuBiomesOn, vbos::VboIdText::kBiome},
          UiStaticSprite{vbos::VboIdMain::kMenuTilesOn, vbos::VboIdText::kTiles},

          UiStaticSprite{vbos::VboIdMain::kMenuSettings, vbos::VboIdText::kNone,
                         [this]() {
                           this->ShowSettings();
                         }},
          UiStaticSprite{vbos::VboIdMain::kMenuShaderWirebound, vbos::VboIdText::kWirebound},
          UiDynamicSprite{vbos::VboIdMain::kCross, vbos::VboIdText::kNone}),
      ui_settings_(
          UiStaticSprite{vbos::VboIdMain::kConfigDesk, vbos::VboIdText::kNone},
          shared_resources_,
          UiStaticSprite{vbos::VboIdMain::kSettingsResolution, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kSettingsSound, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kSettingsMusic, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kSettingsSensitivity, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kSettingsKeyboard, vbos::VboIdText::kNone},
          UiToggle{
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox1Off, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox1On1, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox1On2, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox1On3, vbos::VboIdText::kNone}
          },
          UiToggle{
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox2Off, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox2On1, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox2On2, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox2On3, vbos::VboIdText::kNone}
          },
          UiToggle{
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox3Off, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox3On1, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox3On2, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox3On3, vbos::VboIdText::kNone}
          },
          UiToggle{
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox4Off, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox4On1, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox4On2, vbos::VboIdText::kNone},
              UiStaticSprite{vbos::VboIdMain::kSettingsCheckBox4On3, vbos::VboIdText::kNone}
          },
          UiDynamicSprite{vbos::VboIdMain::kCross, vbos::VboIdText::kNone}
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

  glUniform1f(shader::kSpriteBrightness, 0.5f);
//  ui_components_.Render(); 11111

  glUniform1f(shader::kSpriteBrightness, 1.0f);
  /*ui_components_.widgets_[selected_mode_idx_]->Render();

  if (visibility_.IsTerrainVisible()) {
    ui_components_.widgets_[11]->Render();
  }
  if (visibility_.IsWaterVisible()) {
    ui_components_.widgets_[12]->Render();
  }
  if (visibility_.IsRoadsVisible()) {
    ui_components_.widgets_[13]->Render();
  }
  if (visibility_.IsFencesVisible()) {
    ui_components_.widgets_[14]->Render();
  }
  if (visibility_.IsPlacementVisible()) {
    ui_components_.widgets_[15]->Render();
  }
  if (visibility_.IsObjectsVisible()) {
    ui_components_.widgets_[16]->Render();
  }*/ //11111
}

void Menu::RenderPicking() {
  shared_resources_.static_sprite_picking_shader_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);
  /// first three for text (we don;t need picking for it)
  ui_tab_menu_.RenderPicking();
  if (show_settings_) {
    ui_settings_.RenderPicking();
  }
  for (int i = 3; i < 18; ++i) {
//    ui_components_.widgets_[i]->RenderPicking();
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
  /*if (global_id < static_cast<int>(vbos::VboIdMain::kMenuTerrain) ||
      global_id > static_cast<int>(vbos::VboIdMain::kMenuSettings)) {
    return false;
  }
  int local_id = static_cast<int>(global_id)
                 - static_cast<int>(vbos::VboIdMain::kMenuTerrain);
  if (local_id < 8) {
    cur_mode_ = modes_[local_id];
    cur_mode_->BindCallbacks();
    selected_mode_idx_ = local_id + 3;
    // TODO: reset prev mode?
  } else if (local_id < 16) {
    visibility_.SetMode(local_id - 8);
  } else if (local_id == 17) {
//    tile_renderer_.shaders.EnableWireboundShaders();
  } else if (local_id == 18) {
//    settings_.Show();
  }*/

  if (!show_settings_) {
    ui_tab_menu_.Press(global_id);
  } else {
    ui_settings_.Press(global_id);
  }
  return true;
}
