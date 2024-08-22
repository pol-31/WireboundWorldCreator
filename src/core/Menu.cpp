#include "Menu.h"

#include <glad/glad.h>

#include "../modes/SharedResources.h"
#include "../common/Vbos.h"

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
          UiButton{GetUiData(UiVboDataMainId::kFullScreen, UiVboDataTextId::kMode)},
          UiButton{GetUiData(UiVboDataMainId::kFullScreen, UiVboDataTextId::kVision)},
          UiButton{GetUiData(UiVboDataMainId::kFullScreen, UiVboDataTextId::kShaders)},
          UiButton{GetUiData(UiVboDataMainId::kModeTerrain, UiVboDataTextId::kTerrain)},
          UiButton{GetUiData(UiVboDataMainId::kModeWater, UiVboDataTextId::kWater)},
          UiButton{GetUiData(UiVboDataMainId::kModeRoads, UiVboDataTextId::kRoads)},
          UiButton{GetUiData(UiVboDataMainId::kModeFences, UiVboDataTextId::kFences)},
          UiButton{GetUiData(UiVboDataMainId::kModePlacement, UiVboDataTextId::kPlacement)},
          UiButton{GetUiData(UiVboDataMainId::kModeObjects, UiVboDataTextId::kObject)},
          UiButton{GetUiData(UiVboDataMainId::kModeBiome, UiVboDataTextId::kBiome)},
          UiButton{GetUiData(UiVboDataMainId::kModeTiles, UiVboDataTextId::kTiles)},
          UiButton{GetUiData(UiVboDataMainId::kVisionTerrain, UiVboDataTextId::kTerrain)},
          UiButton{GetUiData(UiVboDataMainId::kVisionWater, UiVboDataTextId::kWater)},
          UiButton{GetUiData(UiVboDataMainId::kVisionRoads, UiVboDataTextId::kRoads)},
          UiButton{GetUiData(UiVboDataMainId::kVisionFences, UiVboDataTextId::kFences)},
          UiButton{GetUiData(UiVboDataMainId::kVisionPlacement, UiVboDataTextId::kPlacement)},
          UiButton{GetUiData(UiVboDataMainId::kVisionObjects, UiVboDataTextId::kObject)},
          UiButton{GetUiData(UiVboDataMainId::kVisionBiome, UiVboDataTextId::kBiome)},
          UiButton{GetUiData(UiVboDataMainId::kVisionTiles, UiVboDataTextId::kTiles)},
          UiButton{GetUiData(UiVboDataMainId::kWireboundLogo, UiVboDataTextId::kWirebound)}}) {
  Init();
}

void Menu::Init() {
  /// first is terrain mode
  selected_mode_idx_ = 3;
  InitText();
}

void Menu::InitText() {
  for (int i = 0; i < 3; ++i) {
    auto text_offset = buttons_[i].GetVboOffset();
    float width = details::kUiVboDataText[text_offset * 2]
                  - details::kUiVboDataText[text_offset * 2 + 4];
    float height = -details::kUiVboDataText[text_offset * 2 + 1]
                   + details::kUiVboDataText[text_offset * 2 + 3];
    // based on text sprite size
    text_params_[i].scale = glm::vec2{width, height};
  }
  /// "mode", "visibility", "shaders"
  text_params_[0].translate = glm::vec2{-0.6f, 0.05f};
  text_params_[1].translate = glm::vec2{-0.6f, -0.05f};
  text_params_[2].translate = glm::vec2{-0.6f, -0.15f};
}

void Menu::Render() const {
  // TODO: can we bind it only once (internally at text_renderer_)?
  const Shader& shader = text_renderer_.Bind();
  for (int i = 0; i < 3; ++i) {
    shader.SetUniformVec2("translate", 1, glm::value_ptr(text_params_[i].translate));
    shader.SetUniformVec2("scale", 1, glm::value_ptr(text_params_[i].scale));
    glDrawArrays(GL_TRIANGLE_STRIP, buttons_[i].GetTextVboOffset(), 4);
  }

  shared_resources_.static_sprite_shader_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);
  shared_resources_.tex_ui_.Bind();

  shared_resources_.static_sprite_shader_.SetUniform("brightness", 0.5f);
  for (int i = 3; i < 20; ++i) {
    buttons_[i].Render();
  }

  shared_resources_.static_sprite_shader_.SetUniform("brightness", 1.0f);
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
  if (global_id < static_cast<int>(UiVboDataMainId::kModeTerrain) ||
      global_id > static_cast<int>(UiVboDataMainId::kModeTerrain) + 16) {
    mode_hover_hover_ = 0.0f;
    return -1;
  }
  int local_id = static_cast<int>(global_id)
                 - static_cast<int>(UiVboDataMainId::kModeTerrain) + 3;

  /*
   *
1 -> 0b0000'0001
2 -> 0b0000'0010
3 -> 0b0000'0100
4 -> 0b0000'1000
5 -> 0b0001'0000
6 -> 0b0010'0000
   * */
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

    shared_resources_.menu_icon_shader_.SetUniform("hover_factor", mode_hover_hover_);

    buttons_[local_id].Render(); // first 3 are text
    last_hovered_ = local_id;
  }
  return buttons_[local_id].Hover();
}

void Menu::Press(uint32_t global_id) {
  if (global_id < static_cast<int>(UiVboDataMainId::kModeTerrain)) {
    return;
  }
  int local_id = static_cast<int>(global_id)
                 - static_cast<int>(UiVboDataMainId::kModeTerrain);
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
