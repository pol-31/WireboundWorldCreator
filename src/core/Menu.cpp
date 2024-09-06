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
      buttons_({
/// first three are text, so not used as simple rendering, but only for text
          UiButton{VboIdMain::kFullScreen, VboIdText::kMode},
          UiButton{VboIdMain::kFullScreen, VboIdText::kVision},
          UiButton{VboIdMain::kFullScreen, VboIdText::kShaders},
          UiButton{VboIdMain::kModeTerrain, VboIdText::kTerrain},
          UiButton{VboIdMain::kModeWater, VboIdText::kWater},
          UiButton{VboIdMain::kModeRoads, VboIdText::kRoads},
          UiButton{VboIdMain::kModeFences, VboIdText::kFences},
          UiButton{VboIdMain::kModePlacement, VboIdText::kPlacement},
          UiButton{VboIdMain::kModeObjects, VboIdText::kObject},
          UiButton{VboIdMain::kModeBiome, VboIdText::kBiome},
          UiButton{VboIdMain::kModeTiles, VboIdText::kTiles},
          UiButton{VboIdMain::kVisionTerrain, VboIdText::kTerrain},
          UiButton{VboIdMain::kVisionWater, VboIdText::kWater},
          UiButton{VboIdMain::kVisionRoads, VboIdText::kRoads},
          UiButton{VboIdMain::kVisionFences, VboIdText::kFences},
          UiButton{VboIdMain::kVisionPlacement, VboIdText::kPlacement},
          UiButton{VboIdMain::kVisionObjects, VboIdText::kObject},
          UiButton{VboIdMain::kVisionBiome, VboIdText::kBiome},
          UiButton{VboIdMain::kVisionTiles, VboIdText::kTiles},
          UiButton{VboIdMain::kWireboundLogo, VboIdText::kWirebound}}) {
  Init();
}

void Menu::Init() {
  /// first is terrain mode
  selected_mode_idx_ = 3;
  InitText();
}

void Menu::InitText() {
  for (int i = 0; i < 3; ++i) {
    auto text_offset = buttons_[i].GetTextVboOffset();
    float width = details::kUiVboDataText[text_offset * 2]
                  - details::kUiVboDataText[text_offset * 2 + 4];
    float height = details::kUiVboDataText[text_offset * 2 + 3]
                   - details::kUiVboDataText[text_offset * 2 + 1];
    // based on text sprite size
    text_params_[i].scale = glm::vec2{width, height};
  }
  /// "mode", "visibility", "shaders"
  text_params_[0].translate = glm::vec2{-0.6f, 0.05f};
  text_params_[1].translate = glm::vec2{-0.6f, -0.05f};
  text_params_[2].translate = glm::vec2{-0.6f, -0.15f};
}

void Menu::Render() const {
  text_renderer_.Bind();
  for (int i = 0; i < 3; ++i) {
    glUniform2fv(shader::kTextTranslate, 1,
                 glm::value_ptr(text_params_[i].translate));
    glUniform2fv(shader::kTextScale, 1,
                 glm::value_ptr(text_params_[i].scale));
    glDrawArrays(GL_TRIANGLE_STRIP,
                 static_cast<GLint>(buttons_[i].GetTextVboOffset()), 4);
  }

  shared_resources_.static_sprite_shader_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);
  shared_resources_.tex_ui_.Bind();

  glUniform1f(shader::kSpriteBrightness, 0.5f);
  for (int i = 3; i < 20; ++i) {
    buttons_[i].Render();
  }

  glUniform1f(shader::kSpriteBrightness, 1.0f);
  buttons_[selected_mode_idx_].Render();

  if (visibility_.IsTerrainVisible()) {
    buttons_[11].Render();
  }
  if (visibility_.IsWaterVisible()) {
    buttons_[12].Render();
  }
  if (visibility_.IsRoadsVisible()) {
    buttons_[13].Render();
  }
  if (visibility_.IsFencesVisible()) {
    buttons_[14].Render();
  }
  if (visibility_.IsPlacementVisible()) {
    buttons_[15].Render();
  }
  if (visibility_.IsObjectsVisible()) {
    buttons_[16].Render();
  }
}

void Menu::RenderPicking() const {
  shared_resources_.static_sprite_picking_shader_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);
  /// first three for text (we don;t need picking for it)
  for (int i = 3; i < 20; ++i) {
    buttons_[i].RenderPicking(shared_resources_.static_sprite_picking_shader_);
  }
}

int Menu::Hover(uint32_t global_id) {
  // skip first 3 for text
  if (global_id < static_cast<int>(VboIdMain::kModeTerrain) ||
      global_id > static_cast<int>(VboIdMain::kModeTerrain) + 16) {
    mode_hover_hover_ = 0.0f;
    return -1;
  }
  int local_id = static_cast<int>(global_id)
                 - static_cast<int>(VboIdMain::kModeTerrain) + 3;

  if (local_id < 20 &&
      selected_mode_idx_ != local_id &&
      !(local_id > 10 && visibility_.visibility & (1 << (local_id - 11)))) {
    if (last_hovered_ != local_id) {
      mode_hover_hover_ = 0.0f;
    }
    mode_hover_hover_ = std::min(1.0f, mode_hover_hover_ + gDeltaTime * 0.8f);
    shared_resources_.menu_icon_shader_.Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(shared_resources_.vao_ui_);
    shared_resources_.tex_ui_.Bind();
    glUniform1f(shader::kSpriteHoveFactor, mode_hover_hover_);
    buttons_[local_id].Render(); // first 3 are text
    last_hovered_ = local_id;
  }
  return buttons_[local_id].Hover();
}

void Menu::Press(uint32_t global_id) {
  if (global_id < static_cast<int>(VboIdMain::kModeTerrain)) {
    return;
  }
  int local_id = static_cast<int>(global_id)
                 - static_cast<int>(VboIdMain::kModeTerrain);
  if (local_id < 8) {
    cur_mode_ = modes_[local_id];
    cur_mode_->BindCallbacks();
    selected_mode_idx_ = local_id + 3;
    // TODO: reset prev mode?
  } else if (local_id < 16) {
    visibility_.SetMode(local_id - 8);
  } else {
//    tile_renderer_.shaders.EnableWireboundShaders();
  }
}
