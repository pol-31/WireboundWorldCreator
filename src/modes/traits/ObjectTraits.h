#ifndef WIREBOUNDWORLDCREATOR_OBJECTTRAITS_H
#define WIREBOUNDWORLDCREATOR_OBJECTTRAITS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "../../common/BaseInstanceData.h"
#include "../../common/MapPoint.h"

struct ObjectTraits : BaseInstanceData {
  std::vector<MapPoint> map_points;  // translation in GLuint id
  std::vector<glm::uvec2> map_joints;
  std::vector<glm::quat> rotations;
  std::vector<glm::vec3> scales;

  float hp = 100.0f;
  float speed = 1.0f;
  float attack = 1.0f;
  float attack_speed = 1.0f;

  static constexpr int GetTraitsSize() noexcept { return 4; }
};
#endif  // WIREBOUNDWORLDCREATOR_OBJECTTRAITS_H
