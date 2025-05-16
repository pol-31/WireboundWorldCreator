#ifndef WIREBOUNDWORLDCREATOR_SRC_MENU_H_
#define WIREBOUNDWORLDCREATOR_SRC_MENU_H_

#include "../modes/IEditMode.h"
#include "Ui.h"
#include "../common/TextRenderer.h"

class SharedResources;

/// here we switch modes
class Menu {
 public:
  explicit Menu(SharedResources& shared_resources,
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
                IEditMode*& cur_mode);

  // TODO: on switching reset all unsaved in previous, update

  void Render(bool show);

  void RenderPicking();

  int Hover(uint32_t global_id);

  bool Press(uint32_t global_id);
/*
  // per tile
  void How__TerrainWaterRoadsFencesBiomesObjectsPlacement__Do() {
    auto cur_tile = tile_renderer_.GetCurrentTile();
    cur_tile.fences.GetFencesGraph(this->selected_slot_id_); // modify, update

    //TODO: that means each instance_Tile already contains std::vector<> of
    // graph or set of points and it was loaded from external file, so
    // it was somehow serialized before

    //TODO: that means all these modes don't have its own data, but rather
    // mutable ref/ptr, and tile contains everything that should be serialized
  }

  // per map
  void How__Tiles__Do() {
    tile_renderer_.SetTile(id);
    //TODO: here render preview (should be fast - we don't calculate anything,
    // so we can block on call)
  }*/

  void ShowSettings();
  void HideSettings();

 private:
  struct TextParams {
    glm::vec2 translate{0.0f, 0.0f};
    glm::vec2 scale{1.0f, 1.0f};
  };

  void Init();

  void InitText();

  SharedResources& shared_resources_;

  UiTabMenu ui_tab_menu_;
  UiSettings ui_settings_;

  bool show_settings_{false};

//
//  UiStaticSprite text_mode_;
//
//  UiStaticSprite text_visibility_;
//
//  UiStaticSprite text_shaders_;
//  UiStaticSprite btn_shaders_wirebound_;
  // + settings

  /// we have 3 text total: "mode", "visibility", "shaders"
  std::array<TextParams, 3> text_params_;

  const TextRenderer& text_renderer_;

  float mode_hover_hover_{0.0f};
  int last_hovered_{0};

  Visibility& visibility_;
  IEditMode*& cur_mode_;
  // used for adjust brightness before render; id related to buttons_ array
  // can we simply use cur_mode_? idk TODO:
  int selected_mode_idx_{3};
  std::array<IEditMode*, 8> modes_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MENU_H_
