#ifndef WIREBOUNDWORLDCREATOR_SRC_MENU_H_
#define WIREBOUNDWORLDCREATOR_SRC_MENU_H_

#include "../modes/IEditMode.h"

/// here we switch modes
class Menu {
 public:
  explicit Menu(IEditMode*& cur_mode) : cur_mode_(cur_mode) {
  }
  // TODO: on switching reset all unsaved in previous, update
  void Render() {}
  void RenderHover(glm::dvec2 pos) {}
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
  IEditMode*& cur_mode_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MENU_H_
