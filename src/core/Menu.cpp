#include "Menu.h"

#include <glad/glad.h>

#include "../modes/SharedResources.h"
#include "../common/Vbos.h"

Menu::Menu(SharedResources& shared_resources,
           std::uint8_t& visibility,
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
      modes_({terrain_mode, water_mode, roads_mode, fences_mode,
              placement_mode, objects_mode, biomes_mode, tiles_mode}),
      cur_mode_(cur_mode),
      visibility_(visibility),
      /// text is first, then modes, then vision, then shaders (Menu::Press)
      /// TODO; more description
      buttons_({
          {"mode", GetUiData(UiVboDataTextId::kMode)},
          {"visibility", GetUiData(UiVboDataTextId::kVision)},
          {"shaders", GetUiData(UiVboDataTextId::kShaders)},
          {"terrain mode", GetUiData(UiVboDataMainId::kModeTerrain)},
          {"water mode", GetUiData(UiVboDataMainId::kModeWater)},
          {"roads mode", GetUiData(UiVboDataMainId::kModeRoads)},
          {"fences mode", GetUiData(UiVboDataMainId::kModeFences)},
          {"placement mode", GetUiData(UiVboDataMainId::kModePlacement)},
          {"objects mode", GetUiData(UiVboDataMainId::kModeObjects)},
          {"biomes mode", GetUiData(UiVboDataMainId::kModeBiome)},
          {"tiles mode", GetUiData(UiVboDataMainId::kModeTiles)},
          {"terrain visibility", GetUiData(UiVboDataMainId::kVisionTerrain)},
          {"water visibility", GetUiData(UiVboDataMainId::kVisionWater)},
          {"roads visibility", GetUiData(UiVboDataMainId::kVisionRoads)},
          {"fences visibility", GetUiData(UiVboDataMainId::kVisionFences)},
          {"placement visibility", GetUiData(UiVboDataMainId::kVisionPlacement)},
          {"objects visibility", GetUiData(UiVboDataMainId::kVisionObjects)},
          {"biomes visibility", GetUiData(UiVboDataMainId::kVisionBiome)},
          {"tiles visibility", GetUiData(UiVboDataMainId::kVisionTiles)},
          {"Wirebound shaders", GetUiData(UiVboDataMainId::kWireboundLogo)}}) {
}

void Menu::Render() const {
  // TODO; here enough no bind shaders only once, so need to fix
  shared_resources_.static_sprite_shader_.Bind();
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(shared_resources_.vao_text_);
  shared_resources_.tex_text_.Bind();
  for (int i = 0; i < 3; ++i) {
    buttons_[i].Render();
  }
  glBindVertexArray(shared_resources_.vao_ui_);
  shared_resources_.tex_ui_.Bind();

  for (int i = 3; i < 20; ++i) {
    buttons_[i].Render();
  }
}

void Menu::RenderPicking() const {
  shared_resources_.static_sprite_picking_shader_.Bind();
  glBindVertexArray(shared_resources_.vao_text_);
  for (int i = 0; i < 3; ++i) {
    buttons_[i].RenderPicking(shared_resources_.static_sprite_picking_shader_);
  }
  glBindVertexArray(shared_resources_.vao_ui_);
  for (int i = 3; i < 20; ++i) {
    buttons_[i].RenderPicking(shared_resources_.static_sprite_picking_shader_);
  }
}

void Menu::RenderHover(glm::dvec2 pos) const {
  // TODO:
}

void Menu::Press(uint32_t global_id) {
  if (global_id < static_cast<int>(UiVboDataMainId::kModeTerrain)) {
    return;
  }
  // first 3 is nothing, because they are texts
  int local_id = global_id - static_cast<int>(UiVboDataMainId::kModeTerrain);
  if (local_id < 8) {
    cur_mode_ = modes_[local_id];
    cur_mode_->BindCallbacks();
    // TODO: reset prev mode?
  } else if (local_id < 16) {
    // TODO: we have 8 modes and 7 vision masks, so we keen use uint_8t and
    // masks
    std::uint8_t mode = 1 << (local_id - 8);
    visibility_ ^= mode;
  } else {
//    tile_renderer_.shaders.EnableWireboundShaders();
  }

}
