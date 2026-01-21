#ifndef WIREBOUNDWORLDCREATOR_BIOMETRAITS_H
#define WIREBOUNDWORLDCREATOR_BIOMETRAITS_H

#include <vector>

#include "../../common/BaseInstanceData.h"
#include "../../common/MapPoint.h"

struct BiomeTraits : BaseInstanceData {
  std::vector<MapPoint> map_points;
  float wind_angle = 0.0f;
  float wind_speed = 0.0f;
  float sun_x = -1.0f;
  float sun_y = -1.0f;
  float sun_z = 0.0f;
  float sun_r = 1.0f;
  float sun_g = 1.0f;
  float sun_b = 1.0f;

  static constexpr int GetTraitsSize() noexcept { return 8; }
};

#endif  // WIREBOUNDWORLDCREATOR_BIOMETRAITS_H
