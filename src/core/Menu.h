#ifndef WIREBOUNDWORLDCREATOR_SRC_MENU_H_
#define WIREBOUNDWORLDCREATOR_SRC_MENU_H_

#include "../modes/IEditMode.h"
#include "../core/Ui.h"

class SharedResources;

/// here we switch modes
class Menu {
 public:
  explicit Menu(SharedResources& shared_resources,
                std::uint8_t& visibility,
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

  void Render() const;
  void RenderPicking() const;

  void RenderHover(glm::dvec2 pos) const;

  void Press(uint32_t global_id);
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

 private:
  SharedResources& shared_resources_;

  std::array<UiButton, 20> buttons_;
//
//  UiStaticSprite text_mode_;
//  UiStaticSprite btn_mode_terrain_;
//  UiStaticSprite btn_mode_water_;
//  UiStaticSprite btn_mode_roads_;
//  UiStaticSprite btn_mode_fences_;
//  UiStaticSprite btn_mode_placement_;
//  UiStaticSprite btn_mode_objects_;
//  UiStaticSprite btn_mode_biomes_;
//  UiStaticSprite btn_mode_tiles_;
//
//  UiStaticSprite text_visibility_;
//  UiStaticSprite btn_vision_terrain_;
//  UiStaticSprite btn_vision_water_;
//  UiStaticSprite btn_vision_roads_;
//  UiStaticSprite btn_vision_fences_;
//  UiStaticSprite btn_vision_placement_;
//  UiStaticSprite btn_vision_objects_;
//  UiStaticSprite btn_vision_biomes_;
//  UiStaticSprite btn_vision_tiles_;
//
//  UiStaticSprite text_shaders_;
//  UiStaticSprite btn_shaders_wirebound_;

  std::uint8_t& visibility_;
  IEditMode*& cur_mode_;
  std::array<IEditMode*, 8> modes_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MENU_H_
