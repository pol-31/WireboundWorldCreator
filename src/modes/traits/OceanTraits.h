#ifndef WIREBOUNDWORLDCREATOR_OCEANTRAITS_H
#define WIREBOUNDWORLDCREATOR_OCEANTRAITS_H

#include <vector>

#include "../../common/BaseInstanceData.h"
#include "../../common/MapPoint.h"

struct OceanLayerTraits {
  float scale{1.0f};
  float fetch{1.0f};
  float wind{1.0f};
  float spread_blend{0.5f};
  float swell{0.5f};
  float peak_enhancement{0.5f};
  float short_waves_fade{0.5f};
  float lambda{1.0f};
  bool visible = true;
};

struct OceanTraits : BaseInstanceData {
  std::vector<MapPoint> map_points;
  OceanLayerTraits near;
  OceanLayerTraits mid;
  OceanLayerTraits far;
};

#endif  // WIREBOUNDWORLDCREATOR_OCEANTRAITS_H
