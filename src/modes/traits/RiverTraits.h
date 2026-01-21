#ifndef WIREBOUNDWORLDCREATOR_RIVERTRAITS_H
#define WIREBOUNDWORLDCREATOR_RIVERTRAITS_H

#include <vector>

#include "../../common/BaseInstanceData.h"
#include "../../common/GraphBakeConfig.h"
#include "../../common/MapPoint.h"

struct RiverTraits : BaseInstanceData {
  std::vector<MapPoint> map_points;
  std::vector<glm::uvec2> map_joints;
  GraphBakeConfig config;
  float transparency = 0.0f;
  float viscosity = 1.0f;

  static constexpr int GetTraitsSize() noexcept { return 8; }

  GraphBakeConfig GetGraphConfig() const noexcept;
};

#endif  // WIREBOUNDWORLDCREATOR_RIVERTRAITS_H
