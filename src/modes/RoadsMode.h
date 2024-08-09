#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_ROADSMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_ROADSMODE_H_

#include "IEditMode.h"

class RoadsMode final : public IEditMode {
 public:
  using IEditMode::IEditMode;

  void Render() override {
    shared_resources_.tile_renderer.RenderPickingTerrain();
    shared_resources_.tile_renderer.Render();
  }
  void BindCallbacks() override {}
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_ROADSMODE_H_
