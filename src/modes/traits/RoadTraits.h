#ifndef WIREBOUNDWORLDCREATOR_ROADTRAITS_H
#define WIREBOUNDWORLDCREATOR_ROADTRAITS_H

#include <vector>

#include "../../common/BaseInstanceData.h"
#include "../../common/GraphBakeConfig.h"
#include "../../common/MapPoint.h"

struct RoadTraits : BaseInstanceData {
  std::vector<MapPoint> map_points;
  std::vector<glm::uvec2> map_joints;
  GraphBakeConfig config;

  static constexpr int GetTraitsSize() noexcept { return 6; }

  GraphBakeConfig GetGraphConfig() const noexcept;
};

#endif  // WIREBOUNDWORLDCREATOR_ROADTRAITS_H
