#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_

#include "IEditMode.h"

class PlacementMode final : public IEditMode {
 public:
  using IEditMode::IEditMode;

  void Render() override {
    // we don't render tile_renderer_.*
  }
  void BindCallbacks() override {}
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_PLACEMENTMODE_H_
