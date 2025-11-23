#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAININSTANCEDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAININSTANCEDATA_H_

#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../common/Texture.h"
#include "../common/BaseInstanceData.h"
#include "TerrainNoiseData.h"

struct TerrainInstanceData {
  NoiseTerrainData data = {}; // hmap inside

  glm::vec3 scale = glm::vec3{1.0f};
  glm::quat rotate = glm::quat{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 translate = glm::vec3{0.0f};

  bool do_tiling = false;
  bool do_invert = false;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAININSTANCEDATA_H_
