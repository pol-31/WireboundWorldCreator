#ifndef WIREBOUNDWORLDCREATOR_MAPPOINT_H
#define WIREBOUNDWORLDCREATOR_MAPPOINT_H

#include <glad/glad.h>

#include <glm/glm.hpp>

struct MapPoint {
  GLuint position = 0;
  bool selected;
};

#endif  // WIREBOUNDWORLDCREATOR_MAPPOINT_H
