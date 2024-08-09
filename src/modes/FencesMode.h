#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_

#include "IEditMode.h"

class FencesMode final : public IEditMode {
 public:
  using IEditMode::IEditMode;

  void Render() override {
    shared_resources_.tile_renderer.RenderPickingTerrain();
    shared_resources_.tile_renderer.Render();
  }
  void BindCallbacks() override {}
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_FENCESMODE_H_
