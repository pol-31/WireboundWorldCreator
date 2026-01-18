#ifndef WIREBOUNDWORLDCREATOR_MAPPOINT_H
#define WIREBOUNDWORLDCREATOR_MAPPOINT_H

#include <glm/glm.hpp>

struct MapPoint {
  glm::vec2 position = glm::vec2(1.0f);
  bool selected = false;
};

#endif  // WIREBOUNDWORLDCREATOR_MAPPOINT_H
